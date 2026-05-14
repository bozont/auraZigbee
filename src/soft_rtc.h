
#include <WatchdogTimer.h>

static uint32_t device_rtc_clock = 0u;
static uint32_t last_millis = 0u;

void rtc_update() {
  uint32_t now = millis();
  uint32_t elapsed = now - last_millis; // rollover-safe

  if (elapsed >= 1000) {
    uint32_t seconds = elapsed / 1000;

    noInterrupts();
    device_rtc_clock += seconds;
    interrupts();

    last_millis += seconds * 1000;
  }
}

void rtc_set_from_unix(time_t unix_ts) {
  noInterrupts();
  device_rtc_clock = unix_ts;
  last_millis = millis();
  interrupts();
}

time_t rtc_get_unix() {
  return device_rtc_clock;
}

static uint8_t rtc_is_leap_year(uint16_t year) {
  return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

uint16_t rtc_get_year(void) {
  uint32_t seconds = device_rtc_clock;
  uint16_t year = 1970;

  while (1) {
    uint32_t year_seconds = rtc_is_leap_year(year) ? 31622400UL : 31536000UL;
    if (seconds < year_seconds) {
      break;
    }
    seconds -= year_seconds;
    year++;
  }

  return year;
}

uint8_t rtc_get_month(void) {
  static const uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  uint32_t seconds = device_rtc_clock;
  uint16_t year = 1970;

  // Remove whole years
  while (1) {
    uint32_t year_seconds = rtc_is_leap_year(year) ? 31622400UL : 31536000UL;
    if (seconds < year_seconds) break;
    seconds -= year_seconds;
    year++;
  }

  // Remove whole months
  for (uint8_t month = 0; month < 12; month++) {
    uint8_t dim = days_in_month[month];
    if (month == 1 && rtc_is_leap_year(year)) dim++; // February leap

    uint32_t month_seconds = dim * 86400UL;
    if (seconds < month_seconds) {
      return month + 1; // 1–12
    }
    seconds -= month_seconds;
  }

  return 1; // fallback
}

uint8_t rtc_get_day(void) {
  static const uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  uint32_t seconds = device_rtc_clock;
  uint16_t year = 1970;

  // Remove years
  while (1) {
    uint32_t year_seconds = rtc_is_leap_year(year) ? 31622400UL : 31536000UL;
    if (seconds < year_seconds) break;
    seconds -= year_seconds;
    year++;
  }

  // Remove months
  for (uint8_t month = 0; month < 12; month++) {
    uint8_t dim = days_in_month[month];
    if (month == 1 && rtc_is_leap_year(year)) dim++;

    uint32_t month_seconds = dim * 86400UL;
    if (seconds < month_seconds) {
      return (seconds / 86400UL) + 1; // 1–31
    }
    seconds -= month_seconds;
  }

  return 1;
}

uint8_t rtc_get_dayname(void) {
  // 1970-01-01 was a Thursday (4)
  return (uint8_t)((device_rtc_clock / 86400UL + 4) % 7);
}

const char* rtc_get_dayname_str(void) {
  static const char* names[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  return names[rtc_get_dayname()];
}

uint8_t rtc_get_second() {
  return device_rtc_clock % 60;
}

uint8_t rtc_get_minute() {
  return (device_rtc_clock / 60) % 60;
}

uint8_t rtc_get_hour() {
  return (device_rtc_clock / 3600) % 24;
}
