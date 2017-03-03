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


#include <Time.h> // see http://playground.arduino.cc/Code/time 
#include <Servo.h>

// delete or mark the next line as comment if you don't need these
//#define CALIBRATION      // enable calibration mode
#define REALTIMECLOCK    // enable real time clock

#define WISHY 3 // Offset of the Y coordinats of the plate-wisher

// When in calibration mode, adjust the following factors until the servos move exactly 90 degrees
#define SERVOFAKTORLEFT 600
#define SERVOFAKTORRIGHT 600

// Zero-position of left and right servo
// When in calibration mode, adjust the NULL-values so that the servo arms are at all times parallel
// either to the X or Y axis
//#define SERVOLEFTNULL 1950
#define SERVOLEFTNULL 1850
#define SERVORIGHTNULL 855

#define SERVOPINLIFT  3
#define SERVOPINLEFT  5
#define SERVOPINRIGHT 6

#define ZOFF 190
// lift positions of lifting servo

#define OFFSET_X -5
#define OFFSET_Y 0
// Move the base position
#define LIFT_DRAW     0
#define LIFT_NODRAW   1
#define LIFT_SWEEPER  2


#define LIFT0_HEIGHT 1110+ZOFF // on drawing surface
#define LIFT1_HEIGHT 950+ZOFF  // between numbers
#define LIFT2_HEIGHT 550+ZOFF  // going towards sweeper

#define ERASER_X 69.0
#define ERASER_Y 49.0


// speed of liftimg arm, higher is slower
#define LIFTSPEED 2000

// length of arms
#define L1 35
#define L2 55.1
#define L3 13.2
#define L4 45

// origin points of left and right servo 
#define O1X 24//22
#define O1Y -25
#define O2X 49//47
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

Servo servo1;  // 
Servo servo2;  // 
Servo servo3;  // 

volatile double lastX = 75;
volatile double lastY = 47.5;

int last_min = 0;

#define EXTERNAL_SWITCH 13

void setup() 
{ 
  
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



    setTime(13,25,0,0,0,0);
  }
#else  
  // Set current time only the first to values, hh,mm are needed
  setTime(14,19,0,0,0,0);
#endif




  drawTo(75.2, 47);
  lift(LIFT_DRAW);
  //servo1.attach(SERVOPINLIFT);  //  lifting servo
  //servo2.attach(SERVOPINLEFT);  //  left servo
  //servo3.attach(SERVOPINRIGHT);  //  right servo
  delay(1000);

} 

void loop() 
{ 

#ifdef CALIBRATION

   if (!servo1.attached()) servo1.attach(SERVOPINLIFT);
    if (!servo2.attached()) servo2.attach(SERVOPINLEFT);
    if (!servo3.attached()) servo3.attach(SERVOPINRIGHT);

  lift(LIFT_NODRAW);

  // Servohorns will have 90° between movements, parallel to x and y axis
  drawTo(-3, 29.2);
  delay(500);
  drawTo(74.1, 28);
  delay(500);

#else 


  if(digitalRead(EXTERNAL_SWITCH)==LOW)
  {
    Serial.println("Sleeeeeeping");
    delay(500);
    return;
  }

  int i = 0;
  if (last_min != minute()) {

    if (!servo1.attached()) servo1.attach(SERVOPINLIFT);
    if (!servo2.attached()) servo2.attach(SERVOPINLEFT);
    if (!servo3.attached()) servo3.attach(SERVOPINRIGHT);

    lift(LIFT_DRAW);

  /*  hour();
    while ((i+1)*10 <= hour())
    {
      i++;
    }*/

    
    number(3+OFFSET_X, 3+OFFSET_Y, 111, 1);
    //number(5+OFFSET_X, 25+OFFSET_Y, i, 0.9);
    number(5+OFFSET_X, 25+OFFSET_Y, hour()/10, 0.9);
    //number(19+OFFSET_X, 25+OFFSET_Y, (hour()-i*10), 0.9);
    number(19+OFFSET_X, 25+OFFSET_Y, hour()%10, 0.9);
    // Central Two points
    number(28+OFFSET_X, 25+OFFSET_Y, 11, 0.9);
//
//    i=0;
//    while ((i+1)*10 <= minute())
//    {
//      i++;
//    }
//    number(34, 25, i, 0.9);
    number(34+OFFSET_X, 25+OFFSET_Y, minute()/10, 0.9);
   // number(48, 25, (minute()-i*10), 0.9);
    number(49+OFFSET_X, 26+OFFSET_Y, minute()%10, 0.9);
    lift(LIFT_SWEEPER);
    
    drawTo(ERASER_X , ERASER_Y);
    
    lift(LIFT_NODRAW);
    last_min = minute();
    delay(580);
    servo1.detach();
    servo2.detach();
    servo3.detach();
  }

#endif

} 



// Writing numeral with bx by being the bottom left originpoint. Scale 1 equals a 20 mm high font.
// The structure follows this principle: move to first startpoint of the numeral, lift down, draw numeral, lift up
void number(float bx, float by, int num, float scale) {
  if(num<10)
    Serial.println(String(num)+"("+String(bx)+","+String(by)+") / "+String(scale));
  
  switch (num) {

  case 0:
    drawTo(bx + 12 * scale, by + 6 * scale);
    lift(LIFT_DRAW);
    bogenGZS(bx + 7 * scale, by + 10 * scale, 10 * scale, -0.8, 6.7, 0.5);
    lift(LIFT_NODRAW);
    break;
  case 1:

    drawTo(bx + 3 * scale, by + 15 * scale);
    lift(LIFT_DRAW);
    drawTo(bx + 10 * scale, by + 20 * scale);
    drawTo(bx + 10 * scale, by + 0 * scale);
    lift(LIFT_NODRAW);
    break;
  case 2:
    drawTo(bx + 2 * scale, by + 12 * scale);
    lift(LIFT_DRAW);
    bogenUZS(bx + 8 * scale, by + 14 * scale, 6 * scale, 3, -0.8, 1);
    drawTo(bx + 1 * scale, by + 0 * scale);
    drawTo(bx + 12 * scale, by + 0 * scale);
    lift(LIFT_NODRAW);
    break;
  case 3:
    drawTo(bx + 2 * scale, by + 17 * scale);
    lift(LIFT_DRAW);
    bogenUZS(bx + 5 * scale, by + 15 * scale, 5 * scale, 3, -2, 1);
    bogenUZS(bx + 5 * scale, by + 5 * scale, 5 * scale, 1.57, -3, 1);
    lift(LIFT_NODRAW);
    break;
  case 4:
    drawTo(bx + 10 * scale, by + 0 * scale);
    lift(LIFT_DRAW);
    drawTo(bx + 10 * scale, by + 20 * scale);
    drawTo(bx + 2 * scale, by + 6 * scale);
    drawTo(bx + 12 * scale, by + 6 * scale);
    lift(LIFT_NODRAW);
    break;
  case 5:
    drawTo(bx + 2 * scale, by + 5 * scale);
    lift(LIFT_DRAW);
    bogenGZS(bx + 5 * scale, by + 6 * scale, 6 * scale, -2.5, 2, 1);
    drawTo(bx + 5 * scale, by + 20 * scale);
    drawTo(bx + 12 * scale, by + 20 * scale);
    lift(LIFT_NODRAW);
    break;
  case 6:
    drawTo(bx + 2 * scale, by + 10 * scale);
    lift(LIFT_DRAW);
    bogenUZS(bx + 7 * scale, by + 6 * scale, 6 * scale, 2, -4.4, 1);
    drawTo(bx + 11 * scale, by + 20 * scale);
    lift(LIFT_NODRAW);
    break;
  case 7:
    drawTo(bx + 2 * scale, by + 20 * scale);
    lift(LIFT_DRAW);
    drawTo(bx + 12 * scale, by + 20 * scale);
    drawTo(bx + 2 * scale, by + 0);
    lift(LIFT_NODRAW);
    break;
  case 8:
    drawTo(bx + 5 * scale, by + 10 * scale);
    lift(LIFT_DRAW);
    bogenUZS(bx + 5 * scale, by + 15 * scale, 5 * scale, 4.7, -1.6, 1);
    bogenGZS(bx + 5 * scale, by + 5 * scale, 5 * scale, -4.7, 2, 1);
    lift(LIFT_NODRAW);
    break;

  case 9:
    drawTo(bx + 9 * scale, by + 11 * scale);
    lift(LIFT_DRAW);
    bogenUZS(bx + 7 * scale, by + 15 * scale, 5 * scale, 4, -0.5, 1);
    drawTo(bx + 5 * scale, by + 0);
    lift(LIFT_NODRAW);
    break;

  // Borrado
  case 111:
    Serial.println("Borrado");


    lift(LIFT_DRAW);
  
    drawTo(70, 45);
// ThoughtFULL ERASE
    for(int i=46;i>26;i=i-3)
    {
      drawTo(65-WISHY, i);
      drawTo(-3, i+1);
    }
    /*
    drawTo(65-WISHY, 43);

    drawTo(65-WISHY, 46);
    drawTo(5, 49);
    drawTo(5, 46);
    drawTo(63-WISHY, 46);
    drawTo(63-WISHY, 42);

    drawTo(5, 42);
    drawTo(5, 38);
    drawTo(63-WISHY, 38);
    drawTo(63-WISHY, 34);

    drawTo(5, 34);
    drawTo(5, 29);
    drawTo(6, 29);
    drawTo(65-WISHY, 26);

    drawTo(5, 26);
    */
    drawTo(60-WISHY, 40);

    drawTo(73.2, 44.0);
    lift(LIFT_SWEEPER);
    delay(500);    
    break;
  //
  case 11:
    Serial.println("Dos puntos");
    drawTo(bx + 5 * scale, by + 15 * scale);
    lift(LIFT_DRAW);
    bogenGZS(bx + 5 * scale, by + 15 * scale, 0.1 * scale, 1, -1, 1);
    delay(10);
    lift(LIFT_NODRAW);
    drawTo(bx + 5 * scale, by + 5 * scale);
    lift(LIFT_DRAW);
    bogenGZS(bx + 5 * scale, by + 5 * scale, 0.1 * scale, 1, -1, 1);
    delay(10);
    lift(LIFT_NODRAW);
    break;

  }
}



void lift(char lift) {
  Serial.println(String("lift ")+(int)(lift));
  switch (lift) {
    // room to optimize  !

  case LIFT_DRAW: //850
      Serial.println("LIFT_DRAW");
      if (servoLift >= LIFT0_HEIGHT) {
      while (servoLift >= LIFT0_HEIGHT) 
      {
        servoLift--;
        servo1.writeMicroseconds(servoLift);				
        delayMicroseconds(LIFTSPEED);
      }
    } 
    else {
      while (servoLift <= LIFT0_HEIGHT) {
        servoLift++;
        servo1.writeMicroseconds(servoLift);
        delayMicroseconds(LIFTSPEED);

      }

    }

    break;

  case LIFT_NODRAW: //150
     Serial.println("LIFT_NODRAW");
    if (servoLift >= LIFT1_HEIGHT) {
      while (servoLift >= LIFT1_HEIGHT) {
        servoLift--;
        servo1.writeMicroseconds(servoLift);
        delayMicroseconds(LIFTSPEED);

      }
    } 
    else {
      while (servoLift <= LIFT1_HEIGHT) {
        servoLift++;
        servo1.writeMicroseconds(servoLift);
        delayMicroseconds(LIFTSPEED);
      }

    }

    break;

  case LIFT_SWEEPER:
    Serial.println("LIFT_SWEEPER"); 
    if (servoLift >= LIFT2_HEIGHT) {
      while (servoLift >= LIFT2_HEIGHT) {
        servoLift--;
        servo1.writeMicroseconds(servoLift);
        delayMicroseconds(LIFTSPEED);
      }
    } 
    else {
      while (servoLift <= LIFT2_HEIGHT) {
        servoLift++;
        servo1.writeMicroseconds(servoLift);				
        delayMicroseconds(LIFTSPEED);
      }
    }
    break;
  }
}


void bogenUZS(float bx, float by, float radius, int start, int ende, float sqee) {
Serial.println("bogenUZS");
  float inkr = -0.05;
  float count = 0;

  do {
    drawTo(sqee * radius * cos(start + count) + bx,
    radius * sin(start + count) + by);
    count += inkr;
  } 
  while ((start + count) > ende);

}

void bogenGZS(float bx, float by, float radius, int start, int ende, float sqee) {
  Serial.println("bogenGZS");
  float inkr = 0.05;
  float count = 0;

  do {
    drawTo(sqee * radius * cos(start + count) + bx,
    radius * sin(start + count) + by);
    count += inkr;
  } 
  while ((start + count) <= ende);
}


void drawTo(double pX, double pY) {
  double dx, dy, c;
  //Serial.println(String("Draw:")+pX+String(",")+pY+String(")"));
  int i;

  // dx dy of new point
  dx = pX - lastX;
  dy = pY - lastY;
  //path lenght in mm, times 4 equals 4 steps per mm
  c = floor(7 * sqrt(dx * dx + dy * dy));

  if (c < 1) c = 1;

  for (i = 0; i <= c; i++) {
    // draw line point by point
    set_XY(lastX + (i * dx / c), lastY + (i * dy / c));

  }

  lastX = pX;
  lastY = pY;
}

double return_angle(double a, double b, double c) {
  // cosine rule for angle between c and a
  return acos((a * a + c * c - b * b) / (2 * a * c));
}

void set_XY(double Tx, double Ty) 
{
  delay(1);
  double dx, dy, c, a1, a2, Hx, Hy;

  // calculate triangle between pen, servoLeft and arm joint
  // cartesian dx/dy
  dx = Tx - O1X;
  dy = Ty - O1Y;

  // polar lemgth (c) and angle (a1)
  c = sqrt(dx * dx + dy * dy); // 
  a1 = atan2(dy, dx); //
  a2 = return_angle(L1, L2, c);

  servo2.writeMicroseconds(floor(((a2 + a1 - M_PI) * SERVOFAKTORLEFT) + SERVOLEFTNULL));

  // calculate joinr arm point for triangle of the right servo arm
  a2 = return_angle(L2, L1, c);
  Hx = Tx + L3 * cos((a1 - a2 + 0.621) + M_PI); //36,5°
  Hy = Ty + L3 * sin((a1 - a2 + 0.621) + M_PI);

  // calculate triangle between pen joint, servoRight and arm joint
  dx = Hx - O2X;
  dy = Hy - O2Y;

  c = sqrt(dx * dx + dy * dy);
  a1 = atan2(dy, dx);
  a2 = return_angle(L1, L4, c);

  servo3.writeMicroseconds(floor(((a1 - a2) * SERVOFAKTORRIGHT) + SERVORIGHTNULL));

}






