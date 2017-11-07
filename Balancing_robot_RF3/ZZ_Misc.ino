


void loadOperatingValues()
{
  EEPROM.get(0, myOperatingValues);
}



void resetDefaultValues()
{
  // hold down pushbutton on start to do this.
}








void defaultOperatingValues()
{
  myOperatingValues.acc_calibration_value = 1300;                            //Enter the accelerometer calibration value (1182)
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
