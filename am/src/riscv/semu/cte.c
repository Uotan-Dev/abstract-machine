#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

extern void __am_get_cur_as(Context *c);
extern void __am_switch(Context *c);

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 0xb: // mcause[XLEN-1] = 0, mcause[XLEN-2:0] = 11
        ev.event = c->GPR1 == (uintptr_t)(-1) ? EVENT_YIELD : EVENT_SYSCALL;
        c->mepc += 4; // Add 4 for riscv and mips
        break;
      default:
        ev.event = EVENT_ERROR;
        break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *ret = (Context *)kstack.end - 1;
  ret->GPRx = (uintptr_t)arg; // a0
  ret->mepc = (uintptr_t)entry;
  ret->pdir = NULL;
#if __riscv_xlen == 64
  ret->mstatus = 0xa00001800;
#else
  ret->mstatus = 0x1800;
#endif
  return ret;
}

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
