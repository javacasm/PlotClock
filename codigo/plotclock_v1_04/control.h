
#ifndef _CONTROL_H_
#define _CONTROL_H_

void attachServos();
void detachServos();
void set_XY(double Tx, double Ty) ;
void drawTo(double pX, double pY);
void lift(char lift);


// Writing numeral with bx by being the bottom left originpoint. Scale 1 equals a 20 mm high font.
// The structure follows this principle: move to first startpoint of the numeral, lift down, draw numeral, lift up
void number(float bx, float by, int num, float scale);

#endif
