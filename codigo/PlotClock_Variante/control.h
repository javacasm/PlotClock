
#ifndef _CONTROL_H_
#define _CONTROL_H_

void attachServos();
void detachServos();

void set_XY(double Tx, double Ty);
double return_angle(double a, double b, double c) ;
void drawTo(double pX, double pY);
void bogenGZS(float bx, float by, float radius, int start, int ende, float sqee);
void bogenUZS(float bx, float by, float radius, int start, int ende, float sqee);
void lift(char lift);
void number(float bx, float by, int num, float scale);
void writeTime(int apo, int bpo, int cpo, int dpo, int epo, int offspo, float scale);
void writeTemperature(int apo, int bpo, int cpo, int dpo, int offspo, float scale);

float temperature();

#endif
