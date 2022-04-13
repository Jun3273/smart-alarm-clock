#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Time.h>
#include <Arduino.h>
#include "draw_display.h"
#include "setup_time.h"
#include "Adafruit_VEML7700.h"
#include <Adafruit_AHTX0.h>
#include "Adafruit_CCS811.h"

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

TFT_eSPI tft = TFT_eSPI();
hw_timer_t* time_timer = NULL;
ESP32Time rtc;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
float txValue = 0;
const int readPin = 14;
bool state = false;
ESP32Time tim;
bool is_set_up = false;
Adafruit_CCS811 ccs;
Adafruit_AHTX0 aht;
Adafruit_VEML7700 veml = Adafruit_VEML7700();
int lux = 0;
int eCO2 = 0;
int tvoc = 0;
sensors_event_t humidityt, tempt;
float humidity = 0;
float temp = 0;
bool alarm_is_set = false;
int red = 13;
int alarm_button = 12;
int snooze_button = 27;

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

Alarm alarm_t;

void addAlarm(int h, int m) {
  alarm_t.hour = h;
  alarm_t.minute = m;
  alarm_is_set = true;
  draw_alarm(true, h, m);
  draw_snooze(false, h, m);
}

bool checkAlarm() {
  int cur_h = tim.getHour(true);
  int cur_m = tim.getMinute();

  if(alarm_t.hour == cur_h && alarm_t.minute == cur_m && alarm_is_set) {
    return true;
  }
  return false;
}

void read_sensor_data() {
  lux = veml.readLuxNormalized();
  if (ccs.available()) {
    ccs.readData();
    eCO2 = ccs.geteCO2();
    tvoc = ccs.getTVOC();
  }
  aht.getEvent(&humidityt, &tempt);
  humidity = humidityt.relative_humidity;
  temp = tempt.temperature;
}

void read_draw_data() {
  read_sensor_data();
  draw_data(temp, humidity, tvoc, eCO2, lux);
}
/*
void setup_sensor_timer(hw_timer_t* timer) {
  timer = timerBegin(1, 80, true);
  timerAttachInterrupt(timer, &read_sensor_data, true);
  timerAlarmWrite(timer,1000000, true); // microseconds
  timerAlarmEnable(timer);
}
*/
void ring_alarm() {
  digitalWrite(red, HIGH);
}

void turn_off_alarm() {
  digitalWrite(red, LOW);
}

void IRAM_ATTR switch_alarm() {
  if (alarm_is_set) {
    alarm_is_set = false;
    draw_alarm(false, alarm_t.hour, alarm_t.minute);
    draw_snooze(false, alarm_t.hour, alarm_t.minute);
    turn_off_alarm();
  } else {
    alarm_is_set = true;
    draw_alarm(true, alarm_t.hour, alarm_t.minute);
    draw_snooze(false, alarm_t.hour, alarm_t.minute);
  }
}

void IRAM_ATTR set_snooze() {
  if (checkAlarm()) {// TODO: change to if alarm is ringing
    alarm_is_set = true;
    int mn = alarm_t.minute;   
    int hr = alarm_t.hour;
    draw_snooze(true, hr, mn);
    mn += 9;
    if (mn >= 60) {
      hr++;
      mn -= 60;
      if (hr >= 24) {
        hr -= 24;
      }
    }
    alarm_t.hour = hr;
    alarm_t.minute = mn;
    turn_off_alarm();
  } 
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
            /*
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
            */
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
  Serial.begin(115200);
  setup_display();

  /* Checking for connection */
  if (!veml.begin()) {
    Serial.println("Adafruit VEML7700 not found");
    delay(1000);
    while (1);
  }
  if (! aht.begin()) {
    Serial.println("Adafruit AHT10/AHT20 not found");
    while (1) delay(10);
  }
  if(!ccs.begin()){
    Serial.println("CCS811 not found");
    while(1);
  } 
  while(!ccs.available());
  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_800MS);
  veml.setLowThreshold(10000);
  veml.setHighThreshold(20000);
  veml.interruptEnable(true);

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
  pinMode(red, OUTPUT);
  digitalWrite(red, LOW);
  pinMode(alarm_button, INPUT_PULLUP);
  pinMode(snooze_button, INPUT_PULLUP);
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
    //Serial.print("time: ");
    //Serial.println(tim.getTime());
    //Serial.print("Set Alarm:\n");
    //Serial.print(alarm_t.hour);
    //Serial.print(" ");
    //Serial.print(alarm_t.minute);
    //Serial.print("\n");
    
    if (!is_set_up) {
      setup_cur_time(tim.getSecond(), tim.getMinute(), tim.getHour());
      setup_time_timer(time_timer);
      draw_alarm(false, 0, 0);
      attachInterrupt(alarm_button, switch_alarm, HIGH);
      attachInterrupt(snooze_button, set_snooze, HIGH);
      //setup_sensor_timer(sensor_timer);
      is_set_up = true;
    }
    if (checkAlarm()) {
      ring_alarm();
    }
    read_draw_data();
    // Sensors
    //read_sensor_data();
    /*
    Serial.print("Light: "); Serial.print(lux); Serial.println(" lx");
    Serial.print("CO2: "); Serial.print(eCO2); Serial.println(" ppm");
    Serial.print("TVOC: "); Serial.print(tvoc); Serial.println(" ppb");
    Serial.print("Temperature: "); Serial.print(temp); Serial.println(" C");
    Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %rH");
    */
    //draw_data(temp, humidity, tvoc, eCO2, lux);
  }
  else {
    Serial.println("Client disconnected, waiting a client connection to notify...");
    pServer->getAdvertising()->start();
    //while(!deviceConnected);
  }
  /*
  if(checkAlarm()) {
    timerAlarmEnable(timer);
    Serial.println("its time");
  }
  else {
    timerAlarmDisable(timer);
  }
  */
  delay(1000);
}
