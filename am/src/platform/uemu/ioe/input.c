#include <am.h>
#include <uemu.h>

#include "input-event-codes.h"

#define KEY_ESCAPE KEY_ESC
#define KEY_EQUALS KEY_EQUAL
#define KEY_LEFTBRACKET KEY_LEFTBRACE
#define KEY_RIGHTBRACKET KEY_RIGHTBRACE
#define KEY_RETURN KEY_ENTER
#define KEY_LSHIFT KEY_LEFTSHIFT
#define KEY_RSHIFT KEY_RIGHTSHIFT
#define KEY_PERIOD KEY_DOT
#define KEY_LCTRL KEY_LEFTCTRL
#define KEY_RCTRL KEY_RIGHTCTRL
#define KEY_APPLICATION KEY_APPSELECT
#define KEY_LALT KEY_LEFTALT
#define KEY_RALT KEY_RIGHTALT

#define GOLDFISH_EVENTS_START 0x10002000
#define GOLDFISH_EVENTS_SIZE 0x1000

enum {
    REG_READ = 0x00,
    REG_SET_PAGE = 0x00,
    REG_LEN = 0x04,
    REG_DATA = 0x08,
    PAGE_NAME = 0x00000,
    PAGE_EVBITS = 0x10000,
    PAGE_ABSDATA = 0x20000 | EV_ABS,
};

enum {
    STATE_INIT = 0, /* The device is initialized */
    STATE_BUFFERED, /* Events have been buffered, but no IRQ raised yet */
    STATE_LIVE      /* Events can be sent directly to the kernel */
};

#define macro(name) [KEY_##name] = AM_KEY_##name,

static const uint32_t keycode_linux_to_am[] = {
  AM_KEYS(macro)
};

#undef macro

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t type = inl(GOLDFISH_EVENTS_START + REG_READ);
  while (type && type != EV_KEY)
    type = inl(GOLDFISH_EVENTS_START + REG_READ);
  if (type == 0) {
    kbd->keycode = AM_KEY_NONE;
    kbd->keydown = false;
    return;
  }
  uint32_t code = inl(GOLDFISH_EVENTS_START + REG_READ);
  if (code == 0) {
    kbd->keycode = AM_KEY_NONE;
    kbd->keydown = false;
    return;
  }
  uint32_t value = inl(GOLDFISH_EVENTS_START + REG_READ);
  kbd->keycode = code < sizeof(keycode_linux_to_am) ? keycode_linux_to_am[code] : AM_KEY_NONE; // needs to translate to am keycode
  kbd->keydown = value;
}

void __am_input_config(AM_INPUT_CONFIG_T *cfg) {
  cfg->present = true;
}

void __am_input_init() {
  outl(GOLDFISH_EVENTS_START + REG_SET_PAGE, PAGE_ABSDATA);
  inl(GOLDFISH_EVENTS_START + REG_LEN);
}