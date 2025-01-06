#include <am.h>
#include <semu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  SEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, MMAP_READ | MMAP_WRITE | MMAP_EXEC);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
#if __riscv_xlen == 64
  // RV64 Sv39
  // Not implemented
  assert(0);
#else
  // RV32 Sv32
  PA_Decode PA = (PA_Decode)(uintptr_t)pa;
  VA_Decode VA = (VA_Decode)(uintptr_t)va;
  assert(PA.offset == VA.offset);
  assert(prot != MMAP_NONE);
  PTE *lv1_PTE = (PTE *)((uintptr_t)as->ptr + (VA.VPN_1 << 2));
  if ((*(PTE_Decode *)lv1_PTE).V == 0) {
    void *lv2_PT = pgalloc_usr(4096);
    *(PTE_Decode *)lv1_PTE = (PTE_Decode) {
      .V = 1, .R = 0, .W = 0, .X = 0,
      .PPN = (uintptr_t)lv2_PT >> 12
    };
  }
  PTE *lv2_PTE = (PTE *)(((*(PTE_Decode *)lv1_PTE).PPN << 12) + (VA.VPN_0 << 2));
  *(PTE_Decode *)lv2_PTE = (PTE_Decode) {
    .V = 1,
    .R = prot & MMAP_READ ? 1 : 0,
    .W = prot & MMAP_WRITE ? 1 : 0,
    .X = prot & MMAP_EXEC ? 1 : 0,
    .U = prot & MMAP_USER ? 1 : 0,
    .PPN = (PA.addr - VA.offset) >> 12
  };
#endif
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *ret = (Context *)kstack.end - 1;
  ret->mepc = (uintptr_t)entry;
  ret->pdir = as->ptr;
  ret->mstatus = 0xc080;
  return ret;
}
