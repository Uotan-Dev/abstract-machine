#include <am.h>
#include <uemu.h>

#define CLINT_MTIME_ADDR 0x0200BFF8

#define RTC_BASE 0x10000100
#define RTC_SIZE 0x100
#define RTC_TIME_LOW 0x00
#define RTC_TIME_HIGH 0x04
#define RTC_ALARM_LOW 0x08
#define RTC_ALARM_HIGH 0x0c
#define RTC_IRQ_ENABLED 0x10
#define RTC_CLEAR_ALARM 0x14
#define RTC_ALARM_STATUS 0x18
#define RTC_CLEAR_INTERRUPT 0x1c

static uint64_t boot_time = 0;

void __am_timer_init() {
  boot_time = inl(CLINT_MTIME_ADDR);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = inl(CLINT_MTIME_ADDR) - boot_time;
}

static int is_leap_year(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static const int days_in_month[] = {
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static void timestamp_to_datetime(uint64_t timestamp, AM_TIMER_RTC_T *rtc) {
  uint64_t seconds = timestamp;
  rtc->second = seconds % 60;
  seconds /= 60;
  
  rtc->minute = seconds % 60;
  seconds /= 60;
  
  rtc->hour = seconds % 24;
  uint64_t days = seconds / 24;
  
  int year = 1970;
  while (1) {
    int days_this_year = is_leap_year(year) ? 366 : 365;
    if (days < days_this_year)
      break;
    days -= days_this_year;
    year++;
  }
  rtc->year = year;
  
  int month = 1;
  while (month <= 12) {
    int days_this_month = days_in_month[month - 1];
    if (month == 2 && is_leap_year(year))
      days_this_month = 29;
    
    if (days < days_this_month)
      break;
    days -= days_this_month;
    month++;
  }
  rtc->month = month;
  rtc->day = days + 1;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  uint32_t time_lo = inl(RTC_BASE + RTC_TIME_LOW);
  uint32_t time_hi = inl(RTC_BASE + RTC_TIME_HIGH);
  uint64_t time_ns = (uint64_t)time_lo | (uint64_t)time_hi << 32;

  timestamp_to_datetime(time_ns / 1000000000ULL, rtc);
}
