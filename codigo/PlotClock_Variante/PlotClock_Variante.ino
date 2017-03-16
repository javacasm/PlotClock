// Plot Thermometer & Clock
// This sketch is based on Johannes Heberlein's Plotclock v 1.02 (2014)
// and fully backward compatible
// added features:
// - inline calibration & setup (use x when conected to a 9600 serial terminal)
// - EEPROM storage of the calibration values
// - temperature measurement capability based on a DS18B20
// - Display flip capability
// - switch for multiple function
// enhancements by St. Ries (6/2014)

// units: mm; microseconds; radians
// origin: bottom left of drawing surface
// all libraries see http://playground.arduino.cc
// V1.1 added support for Motion (PIR) sensor or tactile switch to avoid ploting if not required
// Bug Fix in calibration
#include <EEPROMex.h>
#include <Streaming.h>
#include <Time.h> 
#include <Servo.h>
#include <OneWire.h> // for temp measurement

#include "control.h"
#include "setup.h"

boolean CALIBRATION;   // enable calibration mode
int address_CALIBRATION;

#define REALTIMECLOCK    // enable real time clock

OneWire  ds(Temperaturepin);
boolean showTemp;
int address_showTemp;

#define auxPin 7
boolean auxActive = false;
boolean auxHaveBeenActive = false;

#define switchPin 13
#define ledPin 8
boolean switchFunctionPlotOn;
int address_switchFunctionPlotOn;
boolean lastSwitchStatus;
boolean flipPlot;
int address_flipPlot;

// When in calibration mode, adjust the following factor until the servos move exactly 90 degrees
int SERVOFAKTORLEFT = 650;
int SERVOFAKTORRIGHT = 650;

int address_SERVOFAKTORLEFT;
int address_SERVOFAKTORRIGHT;

// Zero-position of left and right servo
// When in calibration mode, adjust the NULL-values so that the servo arms are at all times parallel
// either to the X or Y axis
int SERVOLEFTNULL = 2250;
int SERVORIGHTNULL = 920;

int address_SERVOLEFTNULL;
int address_SERVORIGHTNULL;

// lift positions of lifting servo
int LIFT0  = 1800; //1080 // on drawing surface
int LIFT1 = 1645; // 925  // between numbers
int LIFT2 = 1445; //725  // going towards sweeper

int address_LIFT0;
int address_LIFT1;
int address_LIFT2;

// speed of liftimg arm, higher is slower
#define LIFTSPEED 1500

// length of arms
#define L1 35
#define L2 55.1
#define L3 13.2

// origin points of left and right servo 
#define O1X 22
#define O1Y -25
#define O2X 47
#define O2Y -25


#ifdef REALTIMECLOCK
// for instructions on how to hook up a real time clock,
// see here -> http://www.pjrc.com/teensy/td_libs_DS1307RTC.html
// DS1307RTC works with the DS1307, DS1337 and DS3231 real time clock chips.
// Please run the SetTime example to initialize the time on new RTC chips and begin running.

#include <Wire.h>
#include <DS1307RTC.h> // see http://playground.arduino.cc/Code/time    
#endif

int servoLift = 1500;


volatile double lastX = 75;
volatile double lastY = 47.5;

int last_min = 0;

// Setup procedure
int a;
int cal_step = 10;
// calibration modes
int cal_mode = 99;
#define right_servo_factor 1
#define left_servo_factor 2
#define right_servo_0_position 3
#define left_servo_0_position 4
#define lift0_position 5
#define lift1_position 6
#define lift2_position 7

void setup() 
{ 
  EEPROM.setMemPool(100, EEPROMSizeUno);

  address_SERVOFAKTORLEFT = EEPROM.getAddress(sizeof(int));
  address_SERVOFAKTORRIGHT = EEPROM.getAddress(sizeof(int));
  address_SERVOLEFTNULL = EEPROM.getAddress(sizeof(int));
  address_SERVORIGHTNULL = EEPROM.getAddress(sizeof(int));
  address_LIFT0 = EEPROM.getAddress(sizeof(int));
  address_LIFT1 = EEPROM.getAddress(sizeof(int));
  address_LIFT2 = EEPROM.getAddress(sizeof(int));
  address_CALIBRATION = EEPROM.getAddress(sizeof(byte));
  address_showTemp = EEPROM.getAddress(sizeof(byte));
  address_switchFunctionPlotOn = EEPROM.getAddress(sizeof(byte));
  address_flipPlot = EEPROM.getAddress(sizeof(byte));

  SERVOFAKTORLEFT = EEPROM.readInt(address_SERVOFAKTORLEFT);
  SERVOFAKTORRIGHT = EEPROM.readInt(address_SERVOFAKTORRIGHT);
  SERVOLEFTNULL = EEPROM.readInt(address_SERVOLEFTNULL);
  SERVORIGHTNULL = EEPROM.readInt(address_SERVORIGHTNULL);
  LIFT0  = EEPROM.readInt(address_LIFT0);
  LIFT1 = EEPROM.readInt(address_LIFT1);
  LIFT2 = EEPROM.readInt(address_LIFT2);
  CALIBRATION = EEPROM.read(address_CALIBRATION);
  showTemp = EEPROM.read(address_showTemp);
  switchFunctionPlotOn = EEPROM.read(address_switchFunctionPlotOn);
  flipPlot = EEPROM.read(address_flipPlot);

  pinMode(switchPin,INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(auxPin,INPUT_PULLUP);
  Serial.begin(9600);

  printStatus();

#ifdef REALTIMECLOCK
  //  Serial.begin(9600);
  //while (!Serial) { ; } // wait for serial port to connect. Needed for Leonardo only

  // Set current time only the first to values, hh,mm are needed  
  tmElements_t tm;
  if (RTC.read(tm)) 
  {
    setTime(tm.Hour,tm.Minute,tm.Second,tm.Day,tm.Month,tm.Year);
    Serial.println("DS1307 time is set OK.");
  } 
  else 
  {
    if (RTC.chipPresent())
    {
      Serial.println("DS1307 is stopped.  Please run the SetTime example to initialize the time and begin running.");
    } 
    else 
    {
      Serial.println("DS1307 read error!  Please check the circuitry.");
    } 
    // Set current time only the first to values, hh,mm are needed
    setTime(19,38,0,0,0,0);
  }
#else  
  // Set current time only the first to values, hh,mm are needed
  setTime(19,38,0,0,0,0);
#endif

  drawTo(75.2, 47);
  lift(0);

  attachServos();

  delay(1000);

  lastSwitchStatus = digitalRead(switchPin);
  if (!switchFunctionPlotOn) showTemp = lastSwitchStatus;

  detachServos();
  
} 

void loop() { 

  if (lastSwitchStatus != digitalRead(switchPin)) {
    lastSwitchStatus = !lastSwitchStatus;
    if (!switchFunctionPlotOn) showTemp = lastSwitchStatus;
    printStatus();
  }
  if (digitalRead(auxPin)) auxActive = true;
  if (Serial.available()) 
    a = Serial.read();
  else
    a = 0;
  if (char(a) == 'x') {
    CALIBRATION = !CALIBRATION;
    EEPROM.write(address_CALIBRATION,CALIBRATION);
    printStatus();
    if (!CALIBRATION) {
      detachServos();
    }
    cal_mode = 99;
  }
  if (CALIBRATION) {
    if (cal_mode == 99) {
      calibration_instructions();  
      cal_mode = 0;
    }
    setup_procedure(a, cal_step, cal_mode);
  }
  else {
      if (!(switchFunctionPlotOn && !digitalRead(switchPin))) {
        if (last_min != minute()) {
          if (auxActive)  {
            if (!showTemp) {
              if (!flipPlot) writeTime(5,19,28,34,48,25,0.9);
              else writeTime(62,48,38,33,19,43,-0.9);
            }
            else {
              if (!flipPlot) writeTemperature(5,19,28, 34,25,0.9);
              else writeTemperature(62,48,38,33,43,-0.9);
            }
            auxActive = false;
            auxHaveBeenActive = true;
          } // auxActive
          else {
            if (auxHaveBeenActive && !auxActive)  {  
              attachServos();
              number(3, 3, 111, 1);
              lift(1);
              detachServos();
              auxHaveBeenActive = false;
            } //auxHaveBeenActive && !auxActive
          } // else auxActive
        }// if new minute
      }
  } //else 
} // main

