#ifndef DRAW_DISPLAY_H
#define DRAW_DISPLAY_H

#include <TFT_eSPI.h>
#include <SPI.h>

extern TFT_eSPI tft;

void setup_display();
void draw_time(int hr, int mn, int sc);
void draw_data(int temp, int hum, int co, int co2, int light);

#endif
