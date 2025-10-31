#include <am.h>
#include <uemu.h>

#define CLINT_MTIME_ADDR 0x0200BFF8

static uint64_t boot_time = 0;

void __am_timer_init() {
  boot_time = inl(CLINT_MTIME_ADDR);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = inl(CLINT_MTIME_ADDR) - boot_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
