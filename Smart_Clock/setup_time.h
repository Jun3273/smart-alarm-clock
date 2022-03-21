#ifndef SETUP_TIME_H
#define SETUP_TIME_H

#include "ESP32Time.h"
#include "draw_display.h"

extern ESP32Time rtc;

void setup_cur_time(int sc, int mn, int hr, int dy = 1, int mt = 1, int yr = 2021, int ms = 0);
void setup_time_timer(hw_timer_t* timer);

#endif
