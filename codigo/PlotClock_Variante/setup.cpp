

void setup_procedure(int a,  int &cal_step, int &cal_mode)  {

  if (a==0) return;

  attachServos();

  // Setup mode and adjust
  switch (char(a))  {            
  case '0':
    calibration_instructions();  
    break;
  case '+':
    value_adjust(cal_mode, cal_step);
    break;
  case '-':
    value_adjust(cal_mode, -1 * cal_step);
    break;
  case '1':
    cal_mode = left_servo_0_position;
    servoLeft.writeMicroseconds(SERVOLEFTNULL);
    break;
  case '2':
    cal_mode = right_servo_0_position;
    servoRight.writeMicroseconds(SERVORIGHTNULL);
    break;
  case '3':
    cal_mode = right_servo_factor;
    drawTo(-3, 29.2);
    break;
  case '4':
    cal_mode = left_servo_factor;
    drawTo(74.1, 28);
    break;
  case 'D':
    if (cal_step == 10) cal_step = 1; 
    else cal_step = 10;
    Serial.print("Calibrationstep = ");
    Serial.println(cal_step);
    break;
  case '8':
    lift(2);
    drawTo(75.2, 47);
    break;
  case '9':
    drawTo(30, 30);
    break;
  case '5':
    cal_mode = lift0_position;
    lift(0);
    break;
  case '6':
    cal_mode = lift1_position;
    lift(1);
    break;
  case '7':
    cal_mode = lift2_position;
    lift(2);
    break;
  case 'A':
    SERVOFAKTORLEFT = 650;
    SERVOFAKTORRIGHT = 650;
    SERVOLEFTNULL = 2250;
    SERVORIGHTNULL = 920;
    LIFT0  = 1800;
    LIFT1 = 1645;
    LIFT2 = 1445;
    break;
  case 'B':
    EEPROM.writeInt(address_SERVOFAKTORLEFT, SERVOFAKTORLEFT);
    EEPROM.writeInt(address_SERVOFAKTORRIGHT, SERVOFAKTORRIGHT);
    EEPROM.writeInt(address_SERVOLEFTNULL, SERVOLEFTNULL);
    EEPROM.writeInt(address_SERVORIGHTNULL, SERVORIGHTNULL);
    EEPROM.writeInt(address_LIFT0, LIFT0);
    EEPROM.writeInt(address_LIFT1, LIFT1);
    EEPROM.writeInt(address_LIFT2, LIFT2);
    break;
  case 'C':
    SERVOFAKTORLEFT = EEPROM.readInt(address_SERVOFAKTORLEFT);
    SERVOFAKTORRIGHT = EEPROM.readInt(address_SERVOFAKTORRIGHT);
    SERVOLEFTNULL = EEPROM.readInt(address_SERVOLEFTNULL);
    SERVORIGHTNULL = EEPROM.readInt(address_SERVORIGHTNULL);
    LIFT0  = EEPROM.readInt(address_LIFT0);
    LIFT1 = EEPROM.readInt(address_LIFT1);
    LIFT2 = EEPROM.readInt(address_LIFT2);
    break;
  case 'w':
    flipPlot = !flipPlot;
    EEPROM.write(address_flipPlot,flipPlot);
    break;
  case 'y':
    showTemp = !showTemp;
    EEPROM.write(address_showTemp,showTemp); 
    printStatus();
    calibration_instructions();      
    break;
  case 'z':
    switchFunctionPlotOn = !switchFunctionPlotOn;
    EEPROM.write(address_switchFunctionPlotOn,switchFunctionPlotOn);
    printStatus();
    calibration_instructions();      
    break;
  }
}


void value_adjust(int cal_mode, int cal_step) {

  switch (cal_mode)  {
  case right_servo_0_position:
    SERVORIGHTNULL = SERVORIGHTNULL + cal_step;
    
    servoRight.writeMicroseconds(SERVORIGHTNULL);
    break;
  case left_servo_0_position:  
    SERVOLEFTNULL = SERVOLEFTNULL + cal_step;
    servoLeft.writeMicroseconds(SERVOLEFTNULL);
    break;
  case right_servo_factor:
    SERVOFAKTORRIGHT = SERVOFAKTORRIGHT + cal_step;
    drawTo(-3, 29.2);
    break;
  case left_servo_factor:
    SERVOFAKTORLEFT = SERVOFAKTORLEFT + cal_step;
    drawTo(74.1, 28);
    break;
  case lift0_position:
    LIFT0 = LIFT0 + cal_step;
    Serial.println(String("Lift0:")
    lift(0);
    break;
  case lift1_position:
    LIFT1 = LIFT1 + cal_step;
    lift(1);
    break;
  case lift2_position:
    LIFT2 = LIFT2 + cal_step;
    lift(2);
    break;
  }
}

void printStatus()  {
  Serial << endl;
  Serial << "x - Change Setup mode  --- Setup mode = ";
  if (CALIBRATION) Serial << "On"; 
  else Serial << "Off";
  Serial << endl;
  if (switchFunctionPlotOn) {
    Serial << "Switch enable/disable plot function" << endl;
    Serial << "Plot function " << digitalRead(switchPin) << endl;
    if (showTemp) Serial << "Shows Temperature" << endl;
    else Serial << "Shows Time" << endl;
  }
  else {
    Serial << "Switch toggle Temperature/Time" << endl;
    if (digitalRead(switchPin)) Serial << "Shows Temperature" << endl;
    else Serial << "Shows Time" << endl;
  }
  Serial << "Flip plot " << flipPlot << endl;
}

void calibration_instructions(){
  Serial << endl;
  Serial << F("0 - Menue") << endl;
  Serial << F("+ - increase") << endl;
  Serial << F("- - decrease") << endl;
  Serial << F("1 - SERVOLEFTNULL position; adjust First Pos 3!") << endl;
  Serial << F("2 - SERVORIGHTNULL position; adjust First pos 4!") << endl; 
  Serial << F("3 - left position; adjust SERVOFAKTORRIGHT") << endl;
  Serial << F("4 - right position; adjust SERVOFAKTORLEFT") << endl;
  Serial << F("5 - Lift 0; adjust Lift 0") << LIFT0 << endl;
  ;
  Serial << F("6 - Lift 1; adjust Lift 1") << LIFT1 << endl;
  ;
  Serial << F("7 - Lift 2; adjust Lift 2") << LIFT2 << endl;
  ;
  Serial << F("8 - Upper origin") << endl;
  Serial << F("9 - Mid position") << endl;
  Serial << F("A - Load default calibration values") << endl;
  Serial << F("B - Store curent values") << endl; 
  Serial << F("C - Load stored values") << endl; 
  Serial << F("D - Toggle 10 / 1 step") << endl;
  Serial << F("w - flip plot") << endl;
  if (!switchFunctionPlotOn)   {
    Serial << F("z - Switch enable/disable plot function") << endl;
  }  
  else {
    Serial << F("z - Switch toggle between Time and Temperature") << endl;
    if (showTemp) Serial << F("y - Plot time") << endl;
    else Serial << F("y - Plot temperature") << endl;
  } 
  Serial << F("x - leave/enter Setup mode") << endl;
}




