
#ifndef _CONFIG_H_
#define _CONFIG_H_

#define SERVOPINLIFT  3
#define SERVOPINLEFT  5
#define SERVOPINRIGHT 6

#define EXTERNAL_SWITCH 13

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


#endif
