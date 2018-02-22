
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Angle calculations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void angleCalculation()
{
  angle_acc = getAngleAccelerationFromGyro();           //Calculate the current angle according to the accelerometer (delta from upright)
  if (!startProg && robotUpright()) {                   //If the accelerometer angle is almost 0
    angle_gyro = angle_acc;                                                 //Load the accelerometer angle in the angle_gyro variable
    startProg = true;                                                              //Set the start variable to start the PID controller
    digitalWrite(ENABLE, LOW);
  }
  Wire.beginTransmission(gyro_address);                                     //Start communication with the gyro
  Wire.write(0x43);                                                         //Start reading at register 43
  Wire.endTransmission();                                                   //End the transmission
  Wire.requestFrom(gyro_address, 4);                                        //Request 4 bytes from the gyro
  int gyro_yaw_data_raw = Wire.read() << 8 | Wire.read();                       //Combine the two bytes to make one integer
  int gyro_pitch_data_raw = Wire.read() << 8 | Wire.read();                     //Combine the two bytes to make one integer
  gyro_pitch_data_raw -= gyro_pitch_calibration_value;                      //Add the gyro calibration value
  angle_gyro += gyro_pitch_data_raw * 0.000031;                             //Calculate the traveled during this loop angle and add this to the angle_gyro variable

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //MPU-6050 offset compensation
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Not every gyro is mounted 100% level with the axis of the robot. This can be cause by misalignments during manufacturing of the breakout board.
  //As a result the robot will not rotate at the exact same spot and start to make larger and larger circles.
  //To compensate for this behavior a VERY SMALL angle compensation is needed when the robot is rotating.
  //Try 0.0000003 or -0.0000003 first to see if there is any improvement.

  gyro_yaw_data_raw -= gyro_yaw_calibration_value;                          //Add the gyro calibration value
  //Uncomment the following line to make the compensation active
  //angle_gyro -= gyro_yaw_data_raw * 0.0000003;                            //Compensate the gyro offset when the robot is rotating
  angle_gyro = angle_gyro * 0.9996 + angle_acc * 0.0004;                    //Correct the drift of the gyro angle with the accelerometer angle
}

boolean robotUpright() {
  return (angle_acc > -0.5 && angle_acc < 0.5);
}

float getAngleAccelerationFromGyro()
{
  Wire.beginTransmission(gyro_address);                                     //Start communication with the gyro
  Wire.write(0x3F);                                                         //Start reading at register 3F
  Wire.endTransmission();                                                   //End the transmission
  Wire.requestFrom(gyro_address, 2);                                        //Request 2 bytes from the gyro
  int accelerometer_data_raw = Wire.read() << 8 | Wire.read();                  //Combine the two bytes to make one integer
  accelerometer_data_raw += myOperatingValues.acc_calibration_value;         //Add the accelerometer calibration value
  if (accelerometer_data_raw > 8200)accelerometer_data_raw = 8200;          //Prevent division by zero by limiting the acc data to +/-8200;
  if (accelerometer_data_raw < -8200)accelerometer_data_raw = -8200;        //Prevent division by zero by limiting the acc data to +/-8200;
  return asin((float)accelerometer_data_raw / 8200.0) * 57.296;        //Calculate the current angle according to the accelerometer
}


