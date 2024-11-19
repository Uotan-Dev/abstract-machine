#ifndef RISCV_H__
#define RISCV_H__

#include <stdint.h>

static inline uint8_t  inb(uintptr_t addr) { return *(volatile uint8_t  *)addr; }
static inline uint16_t inw(uintptr_t addr) { return *(volatile uint16_t *)addr; }
static inline uint32_t inl(uintptr_t addr) { return *(volatile uint32_t *)addr; }

static inline void outb(uintptr_t addr, uint8_t  data) { *(volatile uint8_t  *)addr = data; }
static inline void outw(uintptr_t addr, uint16_t data) { *(volatile uint16_t *)addr = data; }
static inline void outl(uintptr_t addr, uint32_t data) { *(volatile uint32_t *)addr = data; }

#define PTE_V 0x01
#define PTE_R 0x02
#define PTE_W 0x04
#define PTE_X 0x08
#define PTE_U 0x10
#define PTE_A 0x40
#define PTE_D 0x80

enum { MODE_U, MODE_S, MODE_M = 3 };
#define MSTATUS_MXR  (1 << 19)
#define MSTATUS_SUM  (1 << 18)

#if __riscv_xlen == 64
#define MSTATUS_SXL  (2ull << 34)
#define MSTATUS_UXL  (2ull << 32)
#else
#define MSTATUS_SXL  0
#define MSTATUS_UXL  0
#endif

typedef union __attribute__((packed)) {
  struct { uintptr_t PPN : 22, ASID : 9, MODE : 1; } dec_SATP;
  struct { uintptr_t UIE: 1, SIE: 1, WPRI: 1, MIE: 1, UPIE: 1, SPIE: 1, WPRI2: 1, MPIE: 1,
                  SPP: 1, WPRI3: 2, MPP: 2, FS: 2, XS: 2, MPRV: 1, SUM: 1, MXR: 1, TVM: 1,
                  TW: 1, TSR: 1, WPRI4: 8, SD: 1; } dec_MSTATUS;
  struct { uintptr_t VALUE : 32; } dec_MTVEC;
  struct { uintptr_t VALUE : 32 ; } dec_MEPC;
  struct { uintptr_t EXCEPTION : 31, INTERRUPT : 1; } dec_MCAUSE;
  uintptr_t val;
} SR_Decode;

typedef union __attribute__((packed)) {
  struct { uintptr_t offset : 12, VPN_0 : 10, VPN_1 : 10; };
  uintptr_t addr;
} VA_Decode;

typedef union __attribute__((packed)) {
  struct { uintptr_t offset : 12, PPN : 20; }; // Not 22 for now
  uintptr_t addr;
} PA_Decode;

// RV32 Sv32 PTE
typedef union __attribute__((packed)) {
  struct { uintptr_t V : 1, R : 1, W : 1, X : 1, U : 1, G : 1, A : 1, D : 1, RSW : 2, PPN : 22; };
  uintptr_t val;
} PTE_Decode;

#endif
