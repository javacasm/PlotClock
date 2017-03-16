// Plotclock
// cc - by Johannes Heberlein 2014
// v 1.02
// thingiverse.com/joo   wiki.fablab-nuernberg.de
// units: mm; microseconds; radians
// origin: bottom left of drawing surface
// time library see http://playground.arduino.cc/Code/time 
// RTC  library see http://playground.arduino.cc/Code/time 
//               or http://www.pjrc.com/teensy/td_libs_DS1307RTC.html  
// Change log:
// 1.01  Release by joo at https://github.com/9a/plotclock
// 1.02  Additional features implemented by Dave (https://github.com/Dave1001/):
//       - added ability to calibrate servofaktor seperately for left and right servos
//       - added code to support DS1307, DS1337 and DS3231 real time clock chips
//       - see http://www.pjrc.com/teensy/td_libs_DS1307RTC.html for how to hook up the real time clock 
// 1.03  Fixed the length bug at the servo2 angle calculation, other fixups
// 1.04  Add constant, separate files
// 1.05  Add DS1302,lcd I2c and OneWire temperature sensor

#define LCD_I2C
#define TEMP_DS18x20
#define REALTIMECLOCK_DS1302    // enable real time clock DS1302
//#define REALTIMECLOCK_DS1307    // enable real time clock DS1307
#define PIR_SENSOR_PIN 2
// delete or mark the next line as comment if you don't need these
//#define CALIBRATION      // enable calibration mode



#ifdef TEMP_DS18x20
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 11

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


#endif





#if defined(LCD_I2C) || defined(REALTIMECLOCK_DS1307)
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#endif

#include <Time.h> // see http://playground.arduino.cc/Code/time 




#include "config.h"
#include "control.h"


#ifdef REALTIMECLOCK_DS1307

#define RTC_BATTERY_VOLTAGE A0

// for instructions on how to hook up a real time clock,
// see here -> http://www.pjrc.com/teensy/td_libs_DS1307RTC.html
// DS1307RTC works with the DS1307, DS1337 and DS3231 real time clock chips.
// Please run the SetTime example to initialize the time on new RTC chips and begin running.
#include <DS1307RTC.h> // see http://playground.arduino.cc/Code/time    
#endif

#ifdef REALTIMECLOCK_DS1302
#include <DS1302RTC.h>

#define RTC_CE_RST    8
#define RTC_IO_DATA   9
#define RTC_CLK       10

// Set pins:  CE, IO,CLK
DS1302RTC RTC(RTC_CE_RST, RTC_IO_DATA, RTC_CLK);

#endif

#ifdef LCD_I2C
LiquidCrystal_I2C lcd(0x27,16,2);
#endif 

int last_min = 0;

void show(String sText,int x, int y)
{
#ifdef LCD_I2C
  lcd.setCursor(x,y);
  lcd.print(sText);
#endif
  debug(sText);
}

void setup() 
{ 
#ifdef TEMP_DS18x20
  sensors.begin();
#endif

#ifdef LCD_I2C
  lcd.init();  
  lcd.backlight();
#endif
  
  pinMode(EXTERNAL_SWITCH, INPUT_PULLUP);
  
  Serial.begin(9600);

#if defined(REALTIMECLOCK_DS1307) || defined(REALTIMECLOCK_DS1302)
  tmElements_t tm;
#endif
  
#ifdef REALTIMECLOCK_DS1307

  //while (!Serial) { ; } // wait for serial port to connect. Needed for Leonardo only

  // Set current time only the first to values, hh,mm are needed  
  debug("Using RTC");
  
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

    setTime(14,19,0,0,0,0);
  }
#else

#ifdef REALTIMECLOCK_DS1302
  if (RTC.haltRTC()) {
    Serial.println("The DS1302 is stopped.  Please run the SetTime");
    Serial.println("example to initialize the time and begin running.");
    Serial.println();
  }
  if (!RTC.writeEN()) {
    Serial.println("The DS1302 is write protected. This normal.");
    Serial.println();
  }


  if (!RTC.read(tm)) {
    setTime(tm.Hour,tm.Minute,tm.Second,tm.Day,tm.Month,tm.Year);}
  else {
    show("DS1302 read error!",0,1);
  }

#else
  debug("Not using RTC");
  // Set current time only the first to values, hh,mm are needed
  setTime(14,19,0,0,0,0);
#endif
#endif
  
#if PIR_SENSOR_PIN
  pinMode(PIR_SENSOR_PIN,INPUT);
#endif

  drawTo(75.2, 47);
  lift(LIFT_DRAW);
  delay(1000);

} 


void loop() 
{ 

  String sTime=String(getFormatedNumber(hour())+":"+getFormatedNumber(minute()));
  show (sTime,0,0);

#ifdef TEMP_DS18x20
  sensors.requestTemperatures(); // Send the command to get temperatures

  float fTemp=sensors.getTempCByIndex(0);
  if(fTemp>-100)   {
    String sTemp=String(fTemp)+"C";
    show(sTemp,0,1);
  }
  else {
    show("T.Err.",0,1);
  }
#endif

#if defined(RTC_BATTERY_VOLTAGE)
  float vBattery=analogRead(RTC_BATTERY_VOLTAGE)*5.0/1023;

  String sVBattery=String("Bat:")+vBattery+"v";
  show(sVBattery,7,0);
#endif
#if defined(PIR_SENSOR_PIN) && defined(LCD_I2C)
if(digitalRead(PIR_SENSOR_PIN)==HIGH) { // Someboy is near
  lcd.backlight();
}
else {
  lcd.noBacklight();
}

#endif

#ifdef CALIBRATION

  attachServos();
   
  lift(LIFT_NODRAW);

  // Servohorns will have 90° between movements, parallel to x and y axis
  drawTo(-3, 29.2);
  delay(500);
  drawTo(74.1, 28);
  delay(500);

  show("Calibrate",7,1);
#else 


  if(digitalRead(EXTERNAL_SWITCH)==LOW)   {
    show("Sleeeping",7,1);
    delay(500);
    return;
  }
  else   {
    show("Drawing",9,1);
  }



  int i = 0;
  if (last_min != minute()) {

    attachServos();
    
    lift(LIFT_DRAW);
    
    number(3+OFFSET_X, 3+OFFSET_Y, 111, 1);
    // Hour
    number(5+OFFSET_X, 25+OFFSET_Y, hour()/10, 0.9);
    number(19+OFFSET_X, 25+OFFSET_Y, hour()%10, 0.9);

    // Central Two points
    number(28+OFFSET_X, 25+OFFSET_Y, 11, 0.9);
    // Minutes
    number(34+OFFSET_X, 25+OFFSET_Y, minute()/10, 0.9);
    number(48+OFFSET_X, 26+OFFSET_Y, minute()%10, 0.9);
    lift(LIFT_SWEEPER);
    
    drawTo(ERASER_X , ERASER_Y);
    
    lift(LIFT_NODRAW);
    last_min = minute();
    delay(580);
    
    detachServos();
  
  }
#endif

} 


