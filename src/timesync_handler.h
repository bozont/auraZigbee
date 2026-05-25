#pragma once

ZigbeeTimeClient zigbee_time;

void onZigbeeTimeUpdated();

class TimesyncHandler {
public:
  static TimesyncHandler& getInstance() {
    static TimesyncHandler instance;
    return instance;
  }

  void begin() {
    zigbee_time.setTimeUpdateCallback(onZigbeeTimeUpdated);
    zigbee_time.begin();
  }

  void task() {
    static ntp_state_e state = ZBTS_INIT;
    static uint32_t last_sync_sec = 0u;
    static const uint32_t SYNC_RECHECK_TIME_SEC = 1u;

    switch (state) {
    case ZBTS_INIT:
      state = ZBTS_SYNC_IN_PROGRESS;
      this->requestZigbeeTimeSync();
      break;

    case ZBTS_SYNC_IN_PROGRESS: {
      static uint32_t no_response_cnt = 0u;
      if (this->checkTimeSyncStatus()) {
        state = ZBTS_SLEEP;
        last_sync_sec = millis() / 1000;
      } else {
        state = ZBTS_SYNC_WAIT_RESPONSE;
        Serial.printf("No time sync response received; checking again in %d seconds...\n", SYNC_RECHECK_TIME_SEC);
        no_response_cnt++;
        if (no_response_cnt >= 5u) {
          Serial.printf("No time sync response received in %u checks; requesting sync again...\n", no_response_cnt);
          no_response_cnt = 0u;
          state = ZBTS_SYNC_IN_PROGRESS;
          this->requestZigbeeTimeSync();
        }
      }
    } break;

    case ZBTS_SYNC_WAIT_RESPONSE: {
      static uint32_t last_sync_check_time = 0u;
      if (millis() - last_sync_check_time >= (uint32_t)SYNC_RECHECK_TIME_SEC * 1000u) {
        last_sync_check_time = millis();
        state = ZBTS_SYNC_IN_PROGRESS;
      }
    } break;

    case ZBTS_SLEEP: {
      uint32_t uptime_sec = millis() / 1000;
      uint32_t next_sync_time = last_sync_sec + ZBTS_SYNC_FREQUENCY_HOURS * 3600u;
      if (uptime_sec >= next_sync_time) {
        state = ZBTS_SYNC_IN_PROGRESS;
        this->requestZigbeeTimeSync();
      }
    } break;

    default:
      state = ZBTS_INIT;
      break;
    }
  }

  void requestZigbeeTimeSync() {
    Serial.println("Requesting Zigbee time sync...");
    zigbee_time_updated = false;
    zigbee_time.requestTime();
  }

  bool checkTimeSyncStatus() {
    if (zigbee_time_updated) {
      Serial.println("Zigbee time sync response received");
      MillisRTC& rtc = MillisRTC::getInstance();
      this->zigbee_time_initial_update_done = true;
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

  void zigbeeTimeUpdated() {
    this->zigbee_time_updated = true;
  }

  bool isInitialTimeUpdateDone() const {
    return this->zigbee_time_initial_update_done;
  }

private:
  static const uint32_t ZBTS_SYNC_FREQUENCY_HOURS = 1u;
  enum ntp_state_e : uint8_t { ZBTS_INIT = 0, ZBTS_SYNC_IN_PROGRESS, ZBTS_SLEEP, ZBTS_SYNC_WAIT_RESPONSE };

  volatile bool zigbee_time_initial_update_done = false;
  volatile bool zigbee_time_updated = false;

  TimesyncHandler() {}

  TimesyncHandler(const TimesyncHandler&) = delete;
  TimesyncHandler& operator=(const TimesyncHandler&) = delete;
};

void onZigbeeTimeUpdated() {
  TimesyncHandler::getInstance().zigbeeTimeUpdated();
}
