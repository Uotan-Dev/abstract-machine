#include <am.h>
#include <uemu.h>

#define SERIAL_MMIO 0x10000000
#define RBR       0x00  // Receive Buffer Register
#define LSR       0x05  // Line Status Register
#define LSR_DR    0x01  // Data Ready bit

#define KEYDOWN_MASK 0x8000

static const uint8_t ascii_keymap[128] = {
  ['\n'] = AM_KEY_RETURN,
  ['\r'] = AM_KEY_RETURN,
  ['\t'] = AM_KEY_TAB,
  [27]   = AM_KEY_ESCAPE,
  [8]    = AM_KEY_BACKSPACE,
  [127]  = AM_KEY_DELETE,
  
  [' ']  = AM_KEY_SPACE,
  
  ['0'] = AM_KEY_0, ['1'] = AM_KEY_1, ['2'] = AM_KEY_2, ['3'] = AM_KEY_3,
  ['4'] = AM_KEY_4, ['5'] = AM_KEY_5, ['6'] = AM_KEY_6, ['7'] = AM_KEY_7,
  ['8'] = AM_KEY_8, ['9'] = AM_KEY_9,
  
  ['a'] = AM_KEY_A, ['b'] = AM_KEY_B, ['c'] = AM_KEY_C, ['d'] = AM_KEY_D,
  ['e'] = AM_KEY_E, ['f'] = AM_KEY_F, ['g'] = AM_KEY_G, ['h'] = AM_KEY_H,
  ['i'] = AM_KEY_I, ['j'] = AM_KEY_J, ['k'] = AM_KEY_K, ['l'] = AM_KEY_L,
  ['m'] = AM_KEY_M, ['n'] = AM_KEY_N, ['o'] = AM_KEY_O, ['p'] = AM_KEY_P,
  ['q'] = AM_KEY_Q, ['r'] = AM_KEY_R, ['s'] = AM_KEY_S, ['t'] = AM_KEY_T,
  ['u'] = AM_KEY_U, ['v'] = AM_KEY_V, ['w'] = AM_KEY_W, ['x'] = AM_KEY_X,
  ['y'] = AM_KEY_Y, ['z'] = AM_KEY_Z,
  
  ['A'] = AM_KEY_A, ['B'] = AM_KEY_B, ['C'] = AM_KEY_C, ['D'] = AM_KEY_D,
  ['E'] = AM_KEY_E, ['F'] = AM_KEY_F, ['G'] = AM_KEY_G, ['H'] = AM_KEY_H,
  ['I'] = AM_KEY_I, ['J'] = AM_KEY_J, ['K'] = AM_KEY_K, ['L'] = AM_KEY_L,
  ['M'] = AM_KEY_M, ['N'] = AM_KEY_N, ['O'] = AM_KEY_O, ['P'] = AM_KEY_P,
  ['Q'] = AM_KEY_Q, ['R'] = AM_KEY_R, ['S'] = AM_KEY_S, ['T'] = AM_KEY_T,
  ['U'] = AM_KEY_U, ['V'] = AM_KEY_V, ['W'] = AM_KEY_W, ['X'] = AM_KEY_X,
  ['Y'] = AM_KEY_Y, ['Z'] = AM_KEY_Z,
  
  ['-']  = AM_KEY_MINUS,      ['_']  = AM_KEY_MINUS,
  ['=']  = AM_KEY_EQUALS,     ['+']  = AM_KEY_EQUALS,
  ['[']  = AM_KEY_LEFTBRACKET,  ['{']  = AM_KEY_LEFTBRACKET,
  [']']  = AM_KEY_RIGHTBRACKET, ['}']  = AM_KEY_RIGHTBRACKET,
  [';']  = AM_KEY_SEMICOLON,  [':']  = AM_KEY_SEMICOLON,
  ['\''] = AM_KEY_APOSTROPHE, ['"']  = AM_KEY_APOSTROPHE,
  [',']  = AM_KEY_COMMA,      ['<']  = AM_KEY_COMMA,
  ['.']  = AM_KEY_PERIOD,     ['>']  = AM_KEY_PERIOD,
  ['/']  = AM_KEY_SLASH,      ['?']  = AM_KEY_SLASH,
  ['\\'] = AM_KEY_BACKSLASH,  ['|']  = AM_KEY_BACKSLASH,
  ['`']  = AM_KEY_GRAVE,      ['~']  = AM_KEY_GRAVE,
  
  ['!'] = AM_KEY_1, ['@'] = AM_KEY_2, ['#'] = AM_KEY_3,
  ['$'] = AM_KEY_4, ['%'] = AM_KEY_5, ['^'] = AM_KEY_6,
  ['&'] = AM_KEY_7, ['*'] = AM_KEY_8, ['('] = AM_KEY_9, [')'] = AM_KEY_0,
};

static struct {
  bool has_key;
  int keycode;
  bool sent_press;
} key_state = {false, AM_KEY_NONE, false};

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  if (key_state.has_key) {
    if (!key_state.sent_press) {
      kbd->keydown = true;
      kbd->keycode = key_state.keycode;
      key_state.sent_press = true;
    } else {
      kbd->keydown = false;
      kbd->keycode = key_state.keycode;
      key_state.has_key = false;
      key_state.sent_press = false;
    }
    return;
  }
  
  unsigned int lsr = inb(SERIAL_MMIO + LSR);
  if (lsr & LSR_DR) {
    unsigned char ch = inb(SERIAL_MMIO + RBR);
    
    int keycode = (ch < 128) ? ascii_keymap[ch] : AM_KEY_NONE;
    
    if (keycode != AM_KEY_NONE) {
      key_state.has_key = true;
      key_state.keycode = keycode;
      key_state.sent_press = false;
      
      kbd->keydown = true;
      kbd->keycode = keycode;
      key_state.sent_press = true;
      return;
    }
  }
  
  kbd->keydown = false;
  kbd->keycode = AM_KEY_NONE;
}

void __am_input_config(AM_INPUT_CONFIG_T *cfg) {
  cfg->present = true;
}