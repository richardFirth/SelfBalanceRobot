
void PIDCalculations()
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //PID controller calculations
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //The balancing robot is angle driven. First the difference between the desired angel (setpoint) and actual angle (process value)
  //is calculated. The self_balance_pid_setpoint variable is automatically changed to make sure that the robot stays balanced all the time.
  //The (pid_setpoint - pid_output * 0.015) part functions as a brake function.
  
  float pid_error_temp =
  
  /*
  angle_gyro - self_balance_pid_setpoint - pid_setpoint;
  if(pid_output > 10 || pid_output < -10)pid_error_temp += pid_output * 0.015 ;
  */
  
  pid_i_mem += myOperatingValues.pid_i_gain * pid_error_temp;                                 //Calculate the I-controller value and add it to the pid_i_mem variable
  if(pid_i_mem > 400)pid_i_mem = 400;                                       //Limit the I-controller to the maximum controller output
  else if(pid_i_mem < -400)pid_i_mem = -400;
  //Calculate the PID output value
  pid_output = myOperatingValues.pid_p_gain * pid_error_temp + pid_i_mem + myOperatingValues.pid_d_gain * (pid_error_temp - pid_last_d_error);
  if(pid_output > 400)pid_output = 400;                                     //Limit the PI-controller to the maximum controller output
  else if(pid_output < -400)pid_output = -400;

  pid_last_d_error = pid_error_temp;                                        //Store the error for the next loop

  if(pid_output < 5 && pid_output > -5)pid_output = 0;                      //Create a dead-band to stop the motors when the robot is balanced

 // if(angle_gyro > 30 || angle_gyro < -30 || startProg == 0 || low_bat == 1){    //If the robot tips over or the start variable is zero or the battery is empty
 if(angle_gyro > 30 || angle_gyro < -30 || ProgStopped || low_bat){
    pid_output = 0;                                                         //Set the PID controller output to 0 so the motors stop moving
    pid_i_mem = 0;                                                          //Reset the I-controller memory
    //startProg = 0;                                                              //Set the start variable to 0
    ProgStopped = true;
    self_balance_pid_setpoint = 0;                                          //Reset the self_balance_pid_setpoint variable
    digitalWrite(ENABLE,HIGH);
  }

}
/*
float angle_gyro, angle_acc, angle, self_balance_pid_setpoint;
float pid_error_temp, pid_i_mem, pid_setpoint, gyro_input, pid_output, pid_last_d_error;
float pid_output_left, pid_output_right;
*/

float getPID_Error()
{
  float pid_error_temp = angle_gyro - self_balance_pid_setpoint - pid_setpoint;
  if(pid_output > 10 || pid_output < -10) pid_error_temp += pid_output * 0.015;
  return pid_error_temp;
}

