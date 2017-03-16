#include <Arduino.h>
#include <Servo.h>

#include "config.h"

#define ERASE_COD 111
#define TWO_POINTS_COD 11

Servo servoLift; 
Servo servoLeft; 
Servo servoRight;


volatile double lastX = 75;
volatile double lastY = 47.5;

int servoLiftPos = 1500;

String getFormatedNumber(int i)
{
  String sResultado=String(i);
  if (i<10)
    sResultado=String("0")+sResultado;

   return sResultado;
}

void debug(String text)
{
  Serial.println(text);
}


void attachServos()
{
    debug("Attach Servos");
    if (!servoLift.attached()) servoLift.attach(SERVOPINLIFT);
    if (!servoLeft.attached()) servoLeft.attach(SERVOPINLEFT);
    if (!servoRight.attached()) servoRight.attach(SERVOPINRIGHT);

}

void    detachServos()
{
    debug("Detach Servos");
    servoLift.detach();
    servoLeft.detach();
    servoRight.detach();
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

  servoLeft.writeMicroseconds(floor(((a2 + a1 - M_PI) * SERVOFAKTORLEFT) + SERVOLEFTNULL));

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

  servoRight.writeMicroseconds(floor(((a1 - a2) * SERVOFAKTORRIGHT) + SERVORIGHTNULL));

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



void lift(char lift) {
  debug(String("lift to:")+(int)(lift));
  
  switch (lift) {
    // room to optimize  !

  case LIFT_DRAW: //850
      debug("LIFT_DRAW");
      if (servoLiftPos >= LIFT0_HEIGHT) {
      while (servoLiftPos >= LIFT0_HEIGHT) 
      {
        servoLiftPos--;
        servoLift.writeMicroseconds(servoLiftPos);        
        delayMicroseconds(LIFTSPEED);
      }
    } 
    else {
      while (servoLiftPos <= LIFT0_HEIGHT) {
        servoLiftPos++;
        servoLift.writeMicroseconds(servoLiftPos);
        delayMicroseconds(LIFTSPEED);

      }

    }

    break;

  case LIFT_NODRAW: //150
     debug("LIFT_NODRAW");
    if (servoLiftPos >= LIFT1_HEIGHT) {
      while (servoLiftPos >= LIFT1_HEIGHT) {
        servoLiftPos--;
        servoLift.writeMicroseconds(servoLiftPos);
        delayMicroseconds(LIFTSPEED);

      }
    } 
    else {
      while (servoLiftPos <= LIFT1_HEIGHT) {
        servoLiftPos++;
        servoLift.writeMicroseconds(servoLiftPos);
        delayMicroseconds(LIFTSPEED);
      }

    }

    break;

  case LIFT_SWEEPER:
    debug("LIFT_SWEEPER"); 
    if (servoLiftPos >= LIFT2_HEIGHT) {
      while (servoLiftPos >= LIFT2_HEIGHT) {
        servoLiftPos--;
        servoLift.writeMicroseconds(servoLiftPos);
        delayMicroseconds(LIFTSPEED);
      }
    } 
    else {
      while (servoLiftPos <= LIFT2_HEIGHT) {
        servoLiftPos++;
        servoLift.writeMicroseconds(servoLiftPos);        
        delayMicroseconds(LIFTSPEED);
      }
    }
    break;
  }
}


void bogenUZS(float bx, float by, float radius, int start, int ende, float sqee) {
  debug("bogenUZS");
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
  debug("bogenGZS");
  float inkr = 0.05;
  float count = 0;

  do {
    drawTo(sqee * radius * cos(start + count) + bx,
    radius * sin(start + count) + by);
    count += inkr;
  } 
  while ((start + count) <= ende);
}


// Writing numeral with bx by being the bottom left originpoint. Scale 1 equals a 20 mm high font.
// The structure follows this principle: move to first startpoint of the numeral, lift down, draw numeral, lift up
void number(float bx, float by, int num, float scale) {
  if(num<10)
    debug(String(num)+"("+String(bx)+","+String(by)+") / "+String(scale));
  
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
  case ERASE_COD:
    debug("ERASING");


    lift(LIFT_DRAW);
  
    drawTo(70, 45);
// ThoughtFULL ERASE
    for(int i=46;i>26;i=i-3)
    {
      drawTo(65-WISHY, i);
      drawTo(-3, i+1);
    }
    
    drawTo(60-WISHY, 40);

    drawTo(73.2, 44.0);
    lift(LIFT_SWEEPER);
    delay(500);    
    debug("ERASED!!");
    break;
  // Central Two points
  case TWO_POINTS_COD:

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






