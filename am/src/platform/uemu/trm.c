#include <am.h>
#include <uemu.h>

extern char _heap_start;

#define SERIAL_MMIO 0x10000000

#define RBR       0x00
#define THR       0x00
#define DLL       0x00
#define DLH       0x01
#define IER       0x01
#define FCR       0x02
#define LCR       0x03
#define MCR       0x04
#define LSR       0x05

#define DTR  0X01
#define RTS  0X02

static void ns16550a_delay(unsigned int loops)
{
  while (loops--) {
    __asm__ volatile ("nop");
  }
}

void __am_16550_putchar(char ch) {
  if (ch == '\n') __am_16550_putchar('\r');

  unsigned int value;

  value = inb(SERIAL_MMIO + LSR);
  while (!(value & 0x60)) {
    ns16550a_delay(100);
    value = inb(SERIAL_MMIO + LSR);
  }

  outl(SERIAL_MMIO + THR, ch);

}

void __am_init_16550(){
  unsigned int divisor;
  divisor = 50000000 / (16 * 115200);

  outl(SERIAL_MMIO + IER, 0x00);

  outl(SERIAL_MMIO + LCR, 0x83); // enable DLAB

  outl(SERIAL_MMIO + DLH, 0x00); // 115200
  outl(SERIAL_MMIO + DLL, divisor); // 115200

  outl(SERIAL_MMIO + LCR, 0x03); // disable DLAB

  outl(SERIAL_MMIO + FCR, 0x01);
  outl(SERIAL_MMIO + MCR, RTS | DTR);
}

int main(const char *args);

Area heap = RANGE(&_heap_start, PMEM_END);
static const char mainargs[MAINARGS_MAX_LEN] = TOSTRING(MAINARGS_PLACEHOLDER); // defined in CFLAGS

void putch(char ch) {
  __am_16550_putchar(ch);
}

#define POWER_OFF_ADDR 0x100000

void halt(int code) {
  volatile uint32_t *const power_off_reg = (uint32_t *)POWER_OFF_ADDR;
  *power_off_reg = ((uint32_t)code << 16) | 0x5555;

  // should not reach here
  while (1);
}

void _trm_init() {
  __am_init_16550();
  int ret = main(mainargs);
  halt(ret);
}
