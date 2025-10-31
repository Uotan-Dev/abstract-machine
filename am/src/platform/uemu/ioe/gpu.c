#include <am.h>
#include <uemu.h>
#include <string.h>

#define FB_WIDTH 1024
#define FB_HEIGHT 768

void __am_gpu_init() {
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  // *cfg = (AM_GPU_CONFIG_T) {
  //   .present = true, .has_accel = false,
  //   .width = 0, .height = 0,
  //   .vmemsz = 0
  // };
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = FB_WIDTH, .height = FB_HEIGHT,
    .vmemsz = FB_HEIGHT * FB_WIDTH * sizeof(uint32_t)
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
  uint32_t screen_width = FB_WIDTH;
  uint32_t screen_height = FB_HEIGHT;
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
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
