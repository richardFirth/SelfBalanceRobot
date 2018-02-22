
void loadOperatingValues()
{
  EEPROM.get(0, myOperatingValues);
}

void loopTimer()
{
  timingProblem = true;
  while (loop_timer > micros()) { //  To make sure every loop is exactly 4 milliseconds a wait loop
    timingProblem = false;  // if the other code takes too long, we never enter the loop, and problem will be true.
  }
  loop_timer = micros() + 4000;   //The angle calculations are tuned for a loop time of 4 milliseconds.
  digitalWrite(I_GAIN, !timingProblem);
  digitalWrite(P_GAIN, timingProblem);
}

void checkForResetValues()
{
  if (digitalRead(PUSH_BUTTON))
  {
    delay(300);
    digitalWrite(P_GAIN, HIGH);  //blue
    if (!digitalRead(PUSH_BUTTON)) return;
    delay(300);
    digitalWrite(I_GAIN, HIGH);  //green
    if (!digitalRead(PUSH_BUTTON)) return;
    delay(300);
    digitalWrite(D_GAIN, HIGH);  //red
    if (!digitalRead(PUSH_BUTTON)) return;
    delay(300);
    resetToDefaultOperatingValues();
    digitalWrite(P_GAIN, LOW);
    digitalWrite(I_GAIN, LOW);
    digitalWrite(D_GAIN, LOW);
  }
}

void SetAllLEDS(int editMode, int P, int I, int D, int Calib) {
  digitalWrite(EDIT_MODE, editMode);
  digitalWrite(P_GAIN, P);
  digitalWrite(I_GAIN, I);
  digitalWrite(D_GAIN, D);
  digitalWrite(CALIB_LED, Calib);
}

void resetToDefaultOperatingValues()
{
  myOperatingValues.acc_calibration_value = 1145;                            //Enter the accelerometer calibration value (1182) 1175
  myOperatingValues.pid_p_gain = 10;                                       //Gain setting for the P-controller (15)
  myOperatingValues.pid_i_gain = 1.5;                                      //Gain setting for the I-controller (1.5)
  myOperatingValues.pid_d_gain = 30;                                       //Gain setting for the D-controller (30)
  myOperatingValues.turning_speed = 30;                                    //Turning speed (20)
  myOperatingValues.max_target_speed = 150;                                //Max target speed (100)
  EEPROM.put(0, myOperatingValues);
}

int getBalanceValue()
{
  Wire.beginTransmission(0x68);
  Wire.write(0x3F);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 2);
  return ((Wire.read() << 8 | Wire.read()) * -1);
}

void checkBatteryVoltage(int rawPinRead)
{
  if ((rawPinRead < lowVoltageSetting) && (rawPinRead  > 425))
  {
    digitalWrite(ENABLE, HIGH);
    lowVoltageAlarm();
  }
}

void lowVoltageAlarm()
{
  while (1) {
    SetAllLEDS(1, 0, 1, 0, 1);
    delay(250);
    SetAllLEDS(0, 1, 0, 1, 0);
    delay(250);
  }
}
