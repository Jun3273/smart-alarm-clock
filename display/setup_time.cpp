z#include "setup_time.h"

int prev_sec = 0;

void setup_cur_time(int sc, int mn, int hr, int dy, int mt, int yr, int ms) {
  rtc.setTime(sc, mn, hr, dy, mt, yr, ms);
  prev_sec = sc;
}

void update_time() {
  int cur_sec = rtc.getSecond();
  if (prev_sec != cur_sec) {
    draw_time(rtc.getHour(), rtc.getMinute(), rtc.getSecond());
    prev_sec = cur_sec;
  }
}

void setup_time_timer(hw_timer_t* timer) {
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &update_time, true);
  timerAlarmWrite(timer, 50000, true);
  timerAlarmEnable(timer);
}
