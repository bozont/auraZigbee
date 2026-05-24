enum ntp_state_e : uint8_t {
  NTP_INIT = 0,
  NTP_SYNC_IN_PROGRESS,
  NTP_SLEEP,
  NTP_SYNC_WAIT_RESPONSE
};

void onZigbeeTimeUpdated();
void ntpTask();
void requestNTPSync();
bool checkTimeSyncStatus();

void onZigbeeTimeUpdated() {
  zigbee_time_updated = true;
}

void ntpTask() {
  static ntp_state_e state = NTP_INIT;
  static uint32_t last_sync_sec = 0u;
  static const uint32_t SYNC_RECHECK_TIME_SEC = 5u;

  switch (state) {
  case NTP_INIT:
    state = NTP_SYNC_IN_PROGRESS;
    requestNTPSync();
    break;

  case NTP_SYNC_IN_PROGRESS: {
    static uint32_t no_response_cnt = 0u;
    if (checkTimeSyncStatus()) {
      state = NTP_SLEEP;
      last_sync_sec = millis() / 1000;
    } else {
      state = NTP_SYNC_WAIT_RESPONSE;
      Serial.printf("No time sync response received; checking again in %d seconds...\n", SYNC_RECHECK_TIME_SEC);
      no_response_cnt++;
      if (no_response_cnt >= 3u) {
        no_response_cnt = 0u;
        state = NTP_SYNC_IN_PROGRESS;
        requestNTPSync();
        Serial.printf("No time sync response received %u times; requesting sync again...\n", no_response_cnt);
      }
    }
  } break;

  case NTP_SYNC_WAIT_RESPONSE: {
    static uint32_t last_sync_check_time = 0u;
    if (millis() - last_sync_check_time >= (uint32_t)SYNC_RECHECK_TIME_SEC * 1000u) {
      last_sync_check_time = millis();
      state = NTP_SYNC_IN_PROGRESS;
    }
  } break;

  case NTP_SLEEP: {
    uint32_t uptime_sec = millis() / 1000;
    uint32_t next_sync_time = last_sync_sec + NTP_SYNC_FREQUENCY_HOURS * 3600u;
    if (uptime_sec >= next_sync_time) {
      state = NTP_SYNC_IN_PROGRESS;
      requestNTPSync();
    }
  } break;

  default:
    state = NTP_INIT;
    break;
  }
}

void requestNTPSync() {
  Serial.println("Requesting Zigbee time sync...");
  zigbee_time_updated = false;
  zigbee_time.requestTime();
}

bool checkTimeSyncStatus() {
  if (zigbee_time_updated) {
    Serial.println("Zigbee time sync response received");
    MillisRTC& rtc = MillisRTC::getInstance();
    zigbee_time_initial_update_done = true;
    uint32_t current_ntp_time = zigbee_time.getLocalUnixTime();
    rtc.set_from_unix(current_ntp_time);
    Serial.printf("Current NTP time: %lu\n", current_ntp_time);

    time_t time = rtc.get_unix();
    struct tm ts;
    // ts = *localtime(&time);
    gmtime_r(&time, &ts);
    char time_buf[100];
    snprintf(time_buf,
             sizeof(time_buf),
             "%04d-%02d-%02d %02d:%02d:%02d",
             ts.tm_year + 1900,
             ts.tm_mon + 1,
             ts.tm_mday,
             ts.tm_hour,
             ts.tm_min,
             ts.tm_sec);
    Serial.printf("Current date and time: %s\n", time_buf);

    Serial.println("Zigbee time sync finished");
    return true;
  }

  yield();
  return false;
}
