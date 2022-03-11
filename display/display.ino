#include "draw_display.h"
#include "setup_time.h"

#define TEST_HR 7
#define TEST_MIN 52
#define TEST_SEC 4

#define TEST_TEMP 50
#define TEST_HUMIDITY 40
#define TEST_CO 500
#define TEST_CO2 400
#define TEST_LIGHT 30

#define TEST_ALARM_HR 5
#define TEST_ALARM_MIN 34

TFT_eSPI tft = TFT_eSPI();
hw_timer_t* timer = NULL;
ESP32Time rtc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // for debugging
  setup_display();
  setup_cur_time(TEST_SEC, TEST_MIN, TEST_HR);
  setup_time_timer(timer);
  draw_data(TEST_TEMP, TEST_HUMIDITY, TEST_CO, TEST_CO2, TEST_LIGHT);
  draw_alarm(true, TEST_ALARM_HR, TEST_ALARM_MIN);
  draw_snooze(true, TEST_ALARM_HR, TEST_ALARM_MIN);
  draw_snooze(false, TEST_ALARM_HR, TEST_ALARM_MIN);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
