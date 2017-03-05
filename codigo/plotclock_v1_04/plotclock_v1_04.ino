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


#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <Time.h> // see http://playground.arduino.cc/Code/time 


// delete or mark the next line as comment if you don't need these
#define CALIBRATION      // enable calibration mode
#define REALTIMECLOCK    // enable real time clock

#include "config.h"
#include "control.h"


#ifdef REALTIMECLOCK
// for instructions on how to hook up a real time clock,
// see here -> http://www.pjrc.com/teensy/td_libs_DS1307RTC.html
// DS1307RTC works with the DS1307, DS1337 and DS3231 real time clock chips.
// Please run the SetTime example to initialize the time on new RTC chips and begin running.


#include <DS1307RTC.h> // see http://playground.arduino.cc/Code/time    
#endif


LiquidCrystal_I2C lcd(0x27,16,2);

int last_min = 0;



void setup() 
{ 

  sensors.begin();

  lcd.init();  
  
  pinMode(EXTERNAL_SWITCH, INPUT_PULLUP);
  Serial.begin(9600);
  
#ifdef REALTIMECLOCK

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

    setTime(14,19,0,0,0,0);
  }
#else  
  // Set current time only the first to values, hh,mm are needed
  setTime(14,19,0,0,0,0);
#endif

  lcd.backlight();
 

  drawTo(75.2, 47);
  lift(LIFT_DRAW);
  delay(1000);

} 

String getFormatedNumber(int i)
{
  String sResultado=String(i);
  if (i<10)
    sResultado=String("0")+sResultado;

   return sResultado;
}

void loop() 
{ 

  lcd.setCursor(0,0);
  lcd.print(getFormatedNumber(hour())+":"+getFormatedNumber(minute()));

  sensors.requestTemperatures(); // Send the command to get temperatures

  lcd.setCursor(0,1);
  lcd.print(String(sensors.getTempCByIndex(0))+"C");

#ifdef CALIBRATION

  attachServos();
   
  lift(LIFT_NODRAW);

  // Servohorns will have 90° between movements, parallel to x and y axis
  drawTo(-3, 29.2);
  delay(500);
  drawTo(74.1, 28);
  delay(500);
  
  lcd.setCursor(7,1);
  lcd.print("Calibrate");
  Serial.println("Calibrating");



#else 


  if(digitalRead(EXTERNAL_SWITCH)==LOW)
  {
    lcd.setCursor(5,1);
    lcd.print("Sleeeeeping");
    Serial.println("Sleeeeeeping");
    delay(500);
    return;
  }
  else
  {
    lcd.setCursor(9,1);
    lcd.print("Drawing");
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


