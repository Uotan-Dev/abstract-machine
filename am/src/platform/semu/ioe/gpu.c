#include <am.h>
#include <semu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t vgactl_data = inl(VGACTL_ADDR);
  uint32_t screen_width = vgactl_data >> 16;
  uint32_t screen_height = vgactl_data & 0xffff;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = screen_width, .height = screen_height,
    .vmemsz = screen_height * screen_width * sizeof(uint32_t)
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x;
  int y = ctl->y;
  int w = ctl->w;
  int h = ctl->h;
  if (!ctl->sync && (w == 0 || h == 0)) {
    return;
  }
  uint32_t vgactl_data = inl(VGACTL_ADDR);
  uint32_t screen_width = vgactl_data >> 16;
  uint32_t screen_height = vgactl_data & 0xffff;
  uint32_t *pix = ctl->pixels;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (int i = y; i < y + h; i++) {
    for (int j = x; j < x + w; j++) {
      if (i >= screen_height || j >= screen_width) {
        continue;
      }
      fb[screen_width * i + j] = pix[(i - y) * w + j - x];
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
