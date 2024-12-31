#include <am.h>
#include <semu.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)

enum { audio_uninit, audio_init, audio_config };

static size_t sbuf_size = -1;

void __am_audio_init() {
  outl(AUDIO_INIT_ADDR, audio_init);
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  cfg->present = true;
  cfg->bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  outl(AUDIO_FREQ_ADDR, ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR, ctrl->samples);
  outl(AUDIO_INIT_ADDR, audio_config);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);
}

static inline void audio_write(uint8_t *buf, int len) {
  static size_t sbuf_idx = 0;
  while ((int)(sbuf_size - inl(AUDIO_COUNT_ADDR)) < len);
  for (int i = 0; i < len; i++) {
    outb(AUDIO_SBUF_ADDR + sbuf_idx, buf[i]);
    sbuf_idx = (sbuf_idx + 1) % sbuf_size;
  }
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  int buffer_size = ctl->buf.end - ctl->buf.start;
  if (sbuf_size == -1) {
    sbuf_size = inl(AUDIO_SBUF_SIZE_ADDR);
  }
  audio_write(ctl->buf.start, buffer_size);
  outl(AUDIO_COUNT_ADDR, inl(AUDIO_COUNT_ADDR) + buffer_size);
}
