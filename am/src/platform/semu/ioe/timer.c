#include <am.h>
#include <semu.h>

#define UPTIME_LO_ADDR (RTC_ADDR + 0x00)
#define UPTIME_HI_ADDR (RTC_ADDR + 0x04)
#define RTC_SEC_ADDR   (RTC_ADDR + 0x08)
#define RTC_MIN_ADDR   (RTC_ADDR + 0x0c)
#define RTC_HOUR_ADDR  (RTC_ADDR + 0x10)
#define RTC_MDAY_ADDR  (RTC_ADDR + 0x14)
#define RTC_MON_ADDR   (RTC_ADDR + 0x18)
#define RTC_YEAR_ADDR  (RTC_ADDR + 0x1c)
#define EPOCH_USEC     (RTC_ADDR + 0x20)
#define EPOCH_SEC_LO   (RTC_ADDR + 0x24)
#define EPOCH_SEC_HI   (RTC_ADDR + 0x28)

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = (uint64_t)inl(RTC_ADDR + 4) << 32 | inl(RTC_ADDR);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = inl(RTC_SEC_ADDR);
  rtc->minute = inl(RTC_MIN_ADDR);
  rtc->hour   = inl(RTC_HOUR_ADDR);
  rtc->day    = inl(RTC_MDAY_ADDR);
  rtc->month  = inl(RTC_MON_ADDR);
  rtc->year   = inl(RTC_YEAR_ADDR);
}
