#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *str) {
  const char *start = str;
  while (*str) {
    str++;
  }
  return str - start;
}

char *strcpy(char *dst, const char *src) {
  char *s = dst;
  while ((*dst++ = *src++));
  return s;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char *d = dst;
  const char *s = src;
  while (n > 0) {
    --n;
    if ((*d++ = *s++) == '\0') {
      break;
    }
  }
  while (n-- > 0) {
    *d++ = '\0';
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  char *s = dst;
  while (*dst) {
    dst++;
  }
  while ((*dst++ = *src++));
  return s;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  if (n == 0) {
    return 0;
  }
  while (n-- != 0 && *s1 == *s2) {
    if (n == 0 || *s1 == '\0') {
      break;
    }
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

void *memset(void *m, int c, size_t n) {
  char *s = (char *)m;
  while (n--) {
    *s++ = (char)c;
  }
  return m;
}

void *memmove(void *dst, const void *src, size_t n) {
  char *d = dst;
  const char *s = src;
  if (s < d && d < s + n) {
    s += n;
    d += n;
    while (n--) {
      *--d = *--s;
    }
  } else {
    while (n--) {
      *d++ = *s++;
    }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *dst = out;
  const char *src = in;
  while (n--) {
    *dst++ = *src++;
  }
  return out;
}

int memcmp(const void *m1, const void *m2, size_t n) {
  unsigned char *s1 = (unsigned char *)m1;
  unsigned char *s2 = (unsigned char *)m2;
  while (n--) {
    if (*s1 != *s2) {
      return *s1 - *s2;
    }
    s1++;
    s2++;
  }
  return 0;
}

#endif
