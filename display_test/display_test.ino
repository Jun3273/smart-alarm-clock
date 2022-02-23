#include <Arduino_GFX_Library.h>

#define TFT_SCK 5
#define TFT_MOSI 18
#define TFT_MISO 19
#define TFT_CS 16
#define TFT_DC 21
#define TFT_RESET 17

void setup() {
  // put your setup code here, to run once:
  Arduino_ESP32SPI bus = Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);
  Arduino_ILI9341 display = Arduino_ILI9341(&bus, TFT_RESET);

  display.begin();
  display.fillScreen(BLUE);
  display.setCursor(20, 20);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("Hello World!");

  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("works....?");
  delay(5000);
}
