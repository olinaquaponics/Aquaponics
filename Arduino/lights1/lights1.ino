//Control System for Olin Aquaponics Teensy

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Total number of grow pins
const int pinsGrowNum = 1;
const int maxGrowTimeBlocks = 5;

// pin number of each grow light
const int pinsGrow[pinsGrowNum]= {13};
int currGrowStatus[pinsGrowNum] = {};

// time blocks for grow lights. 1 row per grow light: {time start (H), status (1,0)}
const float timeGrow[pinsGrowNum][maxGrowTimeBlocks][2]={
  {{0,0},{6,1},{19.4,0},{24,1}}
};


float currHour = 0;

void printTime(){
    DateTime now = rtc.now();

    Serial.print("Current time: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}

void setupTime(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    Serial.print("RTC set to: ");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println(__TIME__);
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  else {
    Serial.println("Initializing time...");
    printTime();
    Serial.println();
  }
}


int updateGrowStatus(){
  
  // returns current status of grow leds

  DateTime now = rtc.now();

  Serial.println();
  Serial.println("Updating grow light status...");
  printTime();

  currHour = now.hour() + (now.minute()/60.);
  Serial.print("currHour: ");
  Serial.println(currHour);
  
  for(int pin = 0; pin < pinsGrowNum; pin++){
    
    // find the current time block by checking if the next time block is later than now
    int currTimeBlock = 0;
    while(timeGrow[pin][currTimeBlock + 1][0] < currHour){ 
      currTimeBlock++; 
      if(currTimeBlock > maxGrowTimeBlocks) {
        Serial.print("ERROR: Pin ");
        Serial.print(pin);
        Serial.println(" reached end of grow blocks");
        return 0;
      }
    }

    Serial.print("time of curr block: ");
    Serial.print(timeGrow[pin][currTimeBlock][0]);
    Serial.print("\ttime of next block: ");
    Serial.println(timeGrow[pin][currTimeBlock + 1][0]);
    
    currGrowStatus[pin] = timeGrow[pin][currTimeBlock][1];

    Serial.print("Pin: ");
    Serial.print(pin);    
    Serial.print("\tTime Block: ");
    Serial.print(currTimeBlock);
    Serial.print("\tStatus: ");
    Serial.println(currGrowStatus[pin]);
    Serial.println();
  }

  return 1;
}

void updateGrowLights(){
  for(int pin = 0; pin < pinsGrowNum; pin++){
    digitalWrite(pinsGrow[pin], currGrowStatus[pin]);
  }
}


void setupPins(){
  for(int i = 0; i < pinsGrowNum; i++){
    pinMode(pinsGrow[i], OUTPUT);
  }
}


void setup() {

  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif

  Serial.begin(57600);

  setupTime();
  setupPins();
}

void loop() {
  updateGrowStatus();
  updateGrowLights();
  
  delay(3000);
}
