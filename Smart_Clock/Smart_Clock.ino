#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Time.h>
#include <Arduino.h>
#include "draw_display.h"
#include "setup_time.h"

/*
Need to insert a few variables and functions into the setup for display
VAR:
  TFT_eSPI tft = TFT_eSPI();
  hw_timer_t* timer = NULL;
  ESP32Time rtc;
FUNC:
  setup_display();
  setup_cur_time(TEST_SEC, TEST_MIN, TEST_HR);
  setup_time_timer(timer);
  draw_data(TEST_TEMP, TEST_HUMIDITY, TEST_CO, TEST_CO2, TEST_LIGHT);
  // These two depending on whether the alarm is set, unset, disabled, or snoozed
  draw_alarm(true, TEST_ALARM_HR, TEST_ALARM_MIN);
  draw_snooze(true, TEST_ALARM_HR, TEST_ALARM_MIN);
*/

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
float txValue = 0;
const int readPin = 14;
bool state = false;
ESP32Time tim;

//std::string rxValue; // Could also make this a global var to access it in loop()

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

typedef struct Alarm {
  int hour;
  int minute;
} Alarm;

Alarm alarms[10];

void addAlarm(int h, int m) {
  for(int i = 0; i < 10; i++) {
    if(alarms[i].hour == -1 && alarms[i].minute == -1) {
      alarms[i].hour = h;
      alarms[i].minute = m;
      break;
    }
  }
}

bool checkAlarms() {
  int cur_h = tim.getHour(true);
  int cur_m = tim.getMinute();
  for(int i = 0; i < 10; i++) {
    if(alarms[i].hour == cur_h && alarms[i].minute == cur_m) {
      return(true);
    }
  }
  return(false);
}

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    
    if (rxValue.length() > 0) {
      Serial.print("\n");
      String currentTime;
      String hour;
      String minute;
      String second;
      for (int i = 0; i < rxValue.length(); i++) {
        switch(rxValue[i]) {
          case 'H' :
            if(rxValue[i + 1] >= 0x30 && rxValue[i + 1] <= 0x39) {
              hour = rxValue[i + 1];
              if(rxValue[i + 2] >= 0x30 && rxValue[i + 2] <= 0x39) {
                hour = hour + rxValue[i + 2];
              }
            }
            break;
          case 'M' :
            if(rxValue[i + 1] >= 0x30 && rxValue[i + 1] <= 0x39) {
              minute = rxValue[i + 1];
              if(rxValue[i + 2] >= 0x30 && rxValue[i + 2] <= 0x39) {
                minute = minute + rxValue[i + 2];
              }
            }
            break;
          case 'S' :
            if(rxValue[i + 1] >= 0x30 && rxValue[i + 1] <= 0x39) {
              second = rxValue[i + 1];
              if(rxValue[i + 2] >= 0x30 && rxValue[i + 2] <= 0x39) {
                second = second + rxValue[i + 2];
              }
            }
            break;
          case 'O' :
              if(rxValue[i + 1] >= 0x30 && rxValue[i + 1] <= 0x39) {
                hour = rxValue[i + 1];
                if(rxValue[i + 2] >= 0x30 && rxValue[i + 2] <= 0x39) { // rxValue[2] char is not a colon
                  hour = hour + rxValue[i + 2];
                  minute = rxValue[i + 4];
                  if(rxValue[i + 5] != 'A' || rxValue[i + 5] != 'P')
                    minute = minute + rxValue[i + 5];
                }
                else {                                                 // rxValue[2] char is a colon
                  minute = rxValue[3];
                  if(rxValue[i + 4] != 'A' || rxValue[i + 4] != 'P')
                    minute = minute + rxValue[i + 4];
                }
              }
            if(rxValue[3] == 'P' || rxValue[4] == 'P' || rxValue[5] == 'P' || rxValue[6] == 'P' || rxValue[7] == 'P') {
              hour = String(hour.toInt() + 12);
            }
            addAlarm(hour.toInt(),minute.toInt());
            break;
          case '1':
              frequency = 10000;
              timerAlarmWrite(timer, frequency, true);
            break;
          case '2':
              frequency = 9000;
              timerAlarmWrite(timer, frequency, true);
            break;
          case '3':
              frequency = 8000;
              timerAlarmWrite(timer, frequency, true);
            break;
        }
    }
    if(rxValue[0] == 'H') {
      tim.setTime(second.toInt(), minute.toInt(), hour.toInt(), 1,1,2022); // second minute hour day month year
      currentTime = hour + minute + second;
      Serial.print("time: ");
      Serial.print(tim.getTime());
      Serial.print("\n");
    }
    }
  }

};
BLEServer *pServer;
void setup() {
  for( int i = 0; i < 10; i ++) {
    alarms[i].hour = -1;
    alarms[i].minute = -1;
  }
  
  Serial.begin(115200);
  BLEDevice::init("Smart Clock"); // Give it a name
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);             
  pCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    txValue = analogRead(readPin);
    char txString[8];
    dtostrf(txValue, 1, 2, txString);
    Serial.println(txValue);
//    pCharacteristic->setValue(&txValue, 1); // To send the integer value
//    pCharacteristic->setValue("Hello!"); // Sending a test message
    pCharacteristic->setValue(txString);
    pCharacteristic->notify(); // Send the value to the app!
    //Serial.print("* Sent Value: ");
    //Serial.println(txString);
    //Serial.println(" *");
    Serial.print("time: ");
    Serial.print(tim.getTime());
    Serial.print("\nSet Alarms:\n");
    for(int i = 0; i < 10; i++) {
        if(alarms[i].hour != -1) {
        Serial.print(alarms[i].hour);
        Serial.print(" ");
        Serial.print(alarms[i].minute);
        Serial.print("\n");
      }
    }
  }
  else {
    Serial.println("Client disconnected, waiting a client connection to notify...");
    pServer->getAdvertising()->start();
    //while(!deviceConnected);
  }
  if(checkAlarms()) {
    timerAlarmEnable(timer);
    Serial.println("its time");
  }
  else {
    timerAlarmDisable(timer);
  }
  delay(1000);
}
