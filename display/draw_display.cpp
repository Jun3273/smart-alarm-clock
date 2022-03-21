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
  int ypos = 85;
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
  tft.setTextColor(0x7E0, TFT_BLACK);
  xpos += tft.drawString("Temperature", xpos, ytemp, font);
  xpos += tft.drawChar(':', xpos, ytemp, font) + space;
  xpos += tft.drawNumber(temp, xpos, ytemp, font);

  xpos = margin;
  tft.setTextColor(0xFBE0, TFT_BLACK);
  xpos += tft.drawString("Humidity", xpos, yhum, font);
  xpos += tft.drawChar(':', xpos, yhum, font) + space;
  xpos += tft.drawNumber(hum, xpos, yhum, font);

  xpos = margin;
  tft.setTextColor(0xF800, TFT_BLACK);
  xpos += tft.drawString("CO", xpos, yco, font);
  xpos += tft.drawChar(':', xpos, yco, font) + space;
  xpos += tft.drawNumber(co, xpos, yco, font);

  xpos = margin;
  tft.setTextColor(0x7FF, TFT_BLACK);
  xpos += tft.drawString("CO2", xpos, yco2, font);
  xpos += tft.drawChar(':', xpos, yco2, font) + space;
  xpos += tft.drawNumber(co2, xpos, yco2, font);

  xpos = margin;
  tft.setTextColor(0x1F, TFT_BLACK);
  xpos += tft.drawString("Light", xpos, ylight, font);
  xpos += tft.drawChar(':', xpos, ylight, font) + space;
  xpos += tft.drawNumber(light, xpos, ylight, font);  

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

void draw_alarm(bool is_on, int hr, int mn) {
  int xpos = 250;
  int ypos = 130;
  int xtime = 247;
  int ytime = 149;
  int font = 2;
  int tfont = 4;

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (is_on) {
    xpos += tft.drawString(" Alarm ", xpos, ypos, font);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("ON  ", xpos, ypos, font);
    if (hr < 10) {
      xtime += tft.drawChar('0', xtime, ytime, tfont);
    }
    xtime += tft.drawNumber(hr, xtime, ytime, tfont);
    xtime += tft.drawChar(':', xtime, ytime - 2, tfont);
    
    if (mn < 10) {
      xtime += tft.drawChar('0', xtime, ytime, tfont);
    }
    xtime += tft.drawNumber(mn, xtime, ytime, tfont);
  } else {
    xpos += tft.drawString("Alarm ", xpos, ypos, font);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("OFF", xpos, ypos, font);
    if (hr < 10) {
      xtime += tft.drawChar('0', xtime, ytime, tfont);
    }
    xtime += tft.drawNumber(hr, xtime, ytime, tfont);
    xtime += tft.drawChar(':', xtime, ytime - 2, tfont);
    
    if (mn < 10) {
      xtime += tft.drawChar('0', xtime, ytime, tfont);
    }
    xtime += tft.drawNumber(mn, xtime, ytime, tfont);
  }
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

void draw_snooze(bool is_snooze, int hr, int mn) {
  int xpos = 255;
  int ypos = 173;
  int font = 2;

  if (is_snooze) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Snoozed", xpos, ypos, font);
    mn += 9;
    if (mn >= 60) {
      hr++;
      mn -= 60;
      if (hr >= 24) {
        hr -= 24;
      }
    }
    draw_alarm(true, hr, mn + 9);
  } else {
    tft.drawString("            ", xpos, ypos, font);    
  }
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}



