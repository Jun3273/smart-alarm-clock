#include "draw_display.h"

/* 
Initial setup for the display and additional drawings to the display
if needed.
*/

void setup_display() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  return;
}

void draw_time(int hr, int mn, int sc) {
  int xpos = 120;
  int ypos = 95;
  int xcolon, xsecs;
  int font = 6;

  if (hr < 10) {
    xpos += tft.drawChar('0', xpos, ypos, font);
  }
  xpos += tft.drawNumber(hr, xpos, ypos, font);
  xcolon = xpos;
  xpos += tft.drawChar(':', xpos, ypos - 2, font);
  
  if (mn < 10) {
    xpos += tft.drawChar('0', xpos, ypos, font);
  }
  xpos += tft.drawNumber(mn, xpos, ypos, font);
  xsecs = xpos;
  if (sc % 2) { // Flash the colons on/off
    tft.setTextColor(0x7BEF, TFT_BLACK);        // Set color to yellow to dim colon
    tft.drawChar(':', xcolon, ypos - 2, font);     // Hour:minute colon
    xpos += tft.drawChar(':', xsecs, ypos - 2, font); // Seconds colon
    tft.setTextColor(TFT_WHITE, TFT_BLACK);    // Set color back to white
  }
  else {
    tft.drawChar(':', xcolon, ypos - 2, font);     // Hour:minute colon
    xpos += tft.drawChar(':', xsecs, ypos - 2, font); // Seconds colon
  }

  //Draw seconds
  if (sc < 10) xpos += tft.drawChar('0', xpos, ypos, font); // Add leading zero
  tft.drawNumber(sc, xpos, ypos, font);  
}

void draw_data(int temp, int hum, int co, int co2, int light) {
  int xpos = 0;
  int ytemp = 10;
  int yhum = 60;
  int yco = 110;
  int yco2 = 160;
  int ylight = 210;
  int font = 2;
  int space = 4;
  int margin = 15;

  xpos = margin;
  tft.setTextColor(0x7E0);
  xpos += tft.drawString("Temperature", xpos, ytemp, font);
  xpos += tft.drawChar(':', xpos, ytemp, font) + space;
  xpos += tft.drawNumber(temp, xpos, ytemp, font);

  xpos = margin;
  tft.setTextColor(0xFBE0);
  xpos += tft.drawString("Humidity", xpos, yhum, font);
  xpos += tft.drawChar(':', xpos, yhum, font) + space;
  xpos += tft.drawNumber(hum, xpos, yhum, font);

  xpos = margin;
  tft.setTextColor(0xF800);
  xpos += tft.drawString("CO", xpos, yco, font);
  xpos += tft.drawChar(':', xpos, yco, font) + space;
  xpos += tft.drawNumber(co, xpos, yco, font);

  xpos = margin;
  tft.setTextColor(0x7FF);
  xpos += tft.drawString("CO2", xpos, yco2, font);
  xpos += tft.drawChar(':', xpos, yco2, font) + space;
  xpos += tft.drawNumber(co2, xpos, yco2, font);

  xpos = margin;
  tft.setTextColor(0x1F);
  xpos += tft.drawString("Light", xpos, ylight, font);
  xpos += tft.drawChar(':', xpos, ylight, font) + space;
  xpos += tft.drawNumber(light, xpos, ylight, font);  

  tft.setTextColor(TFT_WHITE);
}

