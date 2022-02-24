#include "draw_display.h"

/* 
Initial setup for the display and additional drawings to the display
if needed.
*/
void setup_display(TFT_eSPI &tft) {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  return;
}
