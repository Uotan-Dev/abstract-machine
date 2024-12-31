#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdio/printf.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void putchar(int c) {
  _putchar(c);
}

#endif
