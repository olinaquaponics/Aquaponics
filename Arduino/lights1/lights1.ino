#include <TimeLib.h>
#include <Time.h>

//Control System for Olin Aquaponics Teensy

#include <Wire.h>



char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Total number of grow pins
const int pinsGrowNum = 5;
const int maxGrowTimeBlocks = 5;

// pin number of each grow light
const int pinsGrow[pinsGrowNum] = {12,11,10,9,8};
int currGrowStatus[pinsGrowNum] = {};

// time blocks for grow lights. 1 row per grow light: {time start (H), status (1,0)}
const float timeGrow[pinsGrowNum][maxGrowTimeBlocks][2]={
  {{0,0},{6,1},{19.4,0},{24,1}},
  {{0,0},{6,1},{19.4,0},{24,1}},
  {{0,0},{6,1},{19.4,0},{24,1}},
  {{0,0},{6,1},{19.4,0},{24,1}},
  {{0,0},{6,1},{19.4,0},{24,1}}
};

const int pumpNum = 4;                  // Number of pumps
int currPumpOn = 0;
const int pinsPump[4] = {16,15,14,7};  // Teensy pins

int pumpDuration = 10;         // Time on in seconds
int pumpBreakDuration = 1;     // Time off in seconds

//int pumpDuration = 15 * 60;
//int pumpBreakDuration = 15;

bool pumpOn = 0;
long int lastTimePump = 0;


float currHour = 0;

void printTime(){
    time_t t = now();

    Serial.print("Current time: ");
    Serial.print(year(t), DEC);
    Serial.print('/');
    Serial.print(month(t), DEC);
    Serial.print('/');
    Serial.print(day(t), DEC);
//    Serial.print(" (");
//    Serial.print(daysOfTheWeek[dayOfTheWeek(t)]);
//    Serial.print(") ");
    Serial.print(hour(t), DEC);
    Serial.print(':');
    Serial.print(minute(t), DEC);
    Serial.print(':');
    Serial.print(second(t), DEC);
    Serial.println();
}

void setupTime(){
//  Serial.println("Setup Time");
//  
//  if (! rtc.begin()) {
//    Serial.println("Couldn't find RTC");
//    while (1);
//  }
//  
//  if (! rtc.isrunning()) {
//    Serial.println("RTC is NOT running!");
//    // following line sets the RTC to the date & time this sketch was compiled
//    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//
//    Serial.print("RTC set to: ");
//    Serial.print(__DATE__);
//    Serial.print(" ");
//    Serial.println(__TIME__);
//  }
//  else {
  Serial.println("Initializing time...");
  printTime();
  Serial.println();
//  }
}


int updateGrowStatus(){

  // returns current status of grow leds

  time_t t = now();

  Serial.println();
  Serial.println("Updating grow light status...");
  printTime();

  currHour = hour(t) + (minute(t)/60.);
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
  for(int i = 0; i < pumpNum; i++){
    pinMode(pinsPump[i], OUTPUT);
//    digitalWrite(pinsPump[i], HIGH);
  }
}


void updatePumps(){
  time_t t = now();

  
  // If pump is on
  if(pumpOn){
    if(t > (lastTimePump + pumpDuration)){
      // Set lastTimePump to now, turn off pump
       lastTimePump = t;
       digitalWrite(pinsPump[currPumpOn], LOW);
       Serial.print("Turning off pump #");
       Serial.println(currPumpOn);
       pumpOn = 0;
    }
  }

  // If currently on break
  else{
    if(t > (lastTimePump + pumpBreakDuration)){
      // Set lastTimePump to now, increment currPumpOn, turn on pump
      lastTimePump = t;

      currPumpOn ++;
      if(currPumpOn >= pumpNum){ currPumpOn = 0; }

      digitalWrite(pinsPump[currPumpOn], HIGH);
      Serial.print("Turning on pump #");
      Serial.println(currPumpOn);
      pumpOn = 1;
    }
  }

  Serial.print(t);
  Serial.print("\nPump ");
  Serial.print(currPumpOn);
  Serial.print(" is ");
  Serial.println(pumpOn);
}


void setup() {
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif

  Serial.begin(57600);
  Serial.println("Hi, I'm alive.");

//  setupTime();
  setupPins();
}

void loop() {
//  updateGrowStatus();
//  updateGrowLights();

  updatePumps();

  delay(1000);
}
