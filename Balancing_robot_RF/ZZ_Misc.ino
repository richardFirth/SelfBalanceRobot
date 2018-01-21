





void loadOperatingValues()
{
  EEPROM.get(0, myOperatingValues);
}

void loopTimer()
{
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Loop time timer
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //The angle calculations are tuned for a loop time of 4 milliseconds. To make sure every loop is exactly 4 milliseconds a wait loop
  //is created by setting the loop_timer variable to +4000 microseconds every loop.

   timingProblem = true;
  while(loop_timer > micros()){
    timingProblem = false; // if the other code takes too long, we never enter the loop.
  }
  loop_timer = micros() + 4000;
    digitalWrite(I_GAIN,!timingProblem);
    digitalWrite(P_GAIN,timingProblem);
}



void checkForResetValues()
{
  if (digitalRead(PUSH_BUTTON))
  {
    delay(300);
      digitalWrite(P_GAIN,HIGH);   //blue
      if (!digitalRead(PUSH_BUTTON)) return;
    delay(300);
      digitalWrite(I_GAIN,HIGH);   //green
      if (!digitalRead(PUSH_BUTTON)) return;
    delay(300);
      digitalWrite(D_GAIN,HIGH);   //red
      if (!digitalRead(PUSH_BUTTON)) return;
    delay(300);
  
    resetToDefaultOperatingValues();
  
    digitalWrite(P_GAIN,LOW); 
    digitalWrite(I_GAIN,LOW); 
    digitalWrite(D_GAIN,LOW); 
  }
}


void SetAllLEDS(int editMode, int P, int I, int D, int Calib){

setSingleLED(EDIT_MODE, editMode);
setSingleLED(P_GAIN, P);
setSingleLED(I_GAIN, I);
setSingleLED(D_GAIN, D);
setSingleLED(CALIB_LED, Calib);

  
}


void setSingleLED(int LED, int setType)
{
  if (setType == 0) digitalWrite(LED,LOW);
  if (setType == 1) digitalWrite(LED,HIGH);
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
    Wire.requestFrom(0x68,2);
    return ((Wire.read()<<8|Wire.read())*-1);
}
