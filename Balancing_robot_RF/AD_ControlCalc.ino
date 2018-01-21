  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Control calculations
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void controlCalculation()
 { 
  
  pid_output_left = pid_output;                                             //Copy the controller output to the pid_output_left variable for the left motor
  pid_output_right = pid_output;                                            //Copy the controller output to the pid_output_right variable for the right motor

  if(R_TRIG){                                            //If the first bit of the receive byte is set change the left and right variable to turn the robot to the left
    pid_output_left += myOperatingValues.turning_speed;                                       //Increase the left motor speed
    pid_output_right -= myOperatingValues.turning_speed;                                      //Decrease the right motor speed
  } else
  if(L_TRIG){                                            //If the second bit of the receive byte is set change the left and right variable to turn the robot to the right
    pid_output_left -= myOperatingValues.turning_speed;                                       //Decrease the left motor speed
    pid_output_right += myOperatingValues.turning_speed;                                      //Increase the right motor speed
  } else
  if(DOWN_BUTTON){                                            //If the third bit of the receive byte is set change the left and right variable to turn the robot to the right
    if(pid_setpoint > -2.5)pid_setpoint -= 0.05;                            //Slowly change the setpoint angle so the robot starts leaning forewards
    if(pid_output > myOperatingValues.max_target_speed * -1)pid_setpoint -= 0.005;            //Slowly change the setpoint angle so the robot starts leaning forewards
  } else
  if(UP_BUTTON){                                            //If the forth bit of the receive byte is set change the left and right variable to turn the robot to the right
    if(pid_setpoint < 2.5)pid_setpoint += 0.05;                             //Slowly change the setpoint angle so the robot starts leaning backwards
    if(pid_output < myOperatingValues.max_target_speed)pid_setpoint += 0.005;                 //Slowly change the setpoint angle so the robot starts leaning backwards
  } else {                                                    //Slowly reduce the setpoint to zero if no foreward or backward command is given
    if(pid_setpoint > 0.5)pid_setpoint -=0.05;                              //If the PID setpoint is larger then 0.5 reduce the setpoint with 0.05 every loop
    else if(pid_setpoint < -0.5)pid_setpoint +=0.05;                        //If the PID setpoint is smaller then -0.5 increase the setpoint with 0.05 every loop
    else pid_setpoint = 0;                                                  //If the PID setpoint is smaller then 0.5 or larger then -0.5 set the setpoint to 0
  }
  
  //The self balancing point is adjusted when there is not forward or backwards movement from the transmitter. This way the robot will always find it's balancing point
  if(pid_setpoint == 0){                                                    //If the setpoint is zero degrees
    if(pid_output < 0)self_balance_pid_setpoint += 0.0015;                  //Increase the self_balance_pid_setpoint if the robot is still moving forewards
    if(pid_output > 0)self_balance_pid_setpoint -= 0.0015;                  //Decrease the self_balance_pid_setpoint if the robot is still moving backwards
  }
 }
