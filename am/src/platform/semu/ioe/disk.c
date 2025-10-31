#include <am.h>
#include <semu.h>
#include <klib.h>

#define DISK_PRESENT_ADDR (DISK_ADDR + 0x00)
#define DISK_BLKSZ_ADDR   (DISK_ADDR + 0x04)
#define DISK_BLKCNT_ADDR  (DISK_ADDR + 0x08)
#define DISK_READY_ADDR   (DISK_ADDR + 0x0c)
#define DISK_CMD_ADDR     (DISK_ADDR + 0x10)
#define DISK_BLKNO_ADDR   (DISK_ADDR + 0x14)

#define DISK_CMD_READ  0
#define DISK_CMD_WRITE 1

static int blksz = 0;

void __am_disk_config(AM_DISK_CONFIG_T *cfg) {
  cfg->present = inl(DISK_PRESENT_ADDR);
  cfg->blksz = inl(DISK_BLKSZ_ADDR);
  cfg->blkcnt = inl(DISK_BLKCNT_ADDR);
  blksz = cfg->blksz;
}

void __am_disk_status(AM_DISK_STATUS_T *stat) {
  stat->ready = inl(DISK_READY_ADDR);
}

static inline void wait_io() {
  while (!inl(DISK_READY_ADDR));
}

static void blk_write(uint8_t *p, int no) {
  if (blksz == 0) {
    blksz = inl(DISK_BLKSZ_ADDR);
  }
  for (int i = 0; i < blksz; i++) {
    outb(DISK_BLKBUF_ADDR + i, p[i]);
  }
  outl(DISK_BLKNO_ADDR, no);
  outl(DISK_CMD_ADDR, DISK_CMD_WRITE);
  wait_io();
}

static void blk_read(uint8_t *p, int no) {
  if (blksz == 0) {
    blksz = inl(DISK_BLKSZ_ADDR);
  }
  outl(DISK_BLKNO_ADDR, no);
  outl(DISK_CMD_ADDR, DISK_CMD_READ);
  wait_io();
  for (int i = 0; i < blksz; i++) {
    p[i] = inb(DISK_BLKBUF_ADDR + i);
  }
}

static void (*blkio_f[])(uint8_t *, int) = { [DISK_CMD_READ] = blk_read, [DISK_CMD_WRITE] = blk_write };

void __am_disk_blkio(AM_DISK_BLKIO_T *io) {
  for (int i = 0; i < io->blkcnt; i++) {
    blkio_f[io->write](io->buf + i * blksz, io->blkno + i);
  }
}
