# Smart Alarm Clock
This is the code that runs on the ESP32 controlling the display, sensors, and bluetooth functionalities of the alarm clock.
The code is currently separated in each directories depending on the subsection, but will be integrated together later on.

### Display
Few side notes:
- the ILI9341 display is connected to the ESP32 via SPI meaning that basically 6 pins are connected to the MCU excluding VIN and GND
- the ILI9341 power supply can be in range of 3.3 - 5 V
- the 6 pins used to connect in the testing phase are (display to ESP32)
  - CLK to SCK (Pin 5 I think)
  - MISO to MISO (Pin 19)
  - MOSI to MOSI (Pin 18)
  - CS to Pin 16 (this can be changed to any GPIO)
  - D/S to Pin 21 (this can also be changed to other GPIO)
  - RST to Pin 17 (don't think reset is needed, but is there for now just in case)
- the code uses the TFT_eSPI library and in order for it to work and you need to manually edit `TFT_eSPI.h` to comment/uncomment/edit the pins that are being used.
