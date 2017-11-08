///////////////////////////////////////////////////////////////////////////////////////
//Terms of use
///////////////////////////////////////////////////////////////////////////////////////
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////////////





#include <SoftwareSerial.h>  // RF
#include <EEPROM.h>
#include <Wire.h>                                            //Include the Wire.h library so we can communicate with the gyro

int gyro_address = 0x68;                                     //MPU-6050 I2C address (0x68 or 0x69)

#define XBEE_RX 2   //RX Pin For Software Serial
#define XBEE_TX 3   //TX Pin For Software Serial

#define LEFT_DIR 4   //direction
#define LEFT_STP 5   //step
#define RIGHT_DIR 6   //direction
#define RIGHT_STP 7   //step

#define ENABLE 8   //
#define PUSH_BUTTON 9   //

#define VBATT A0
#define CALIB_LED A1   

#define EDIT_MODE 10   //
#define P_GAIN 13   //
#define I_GAIN 12   //
#define D_GAIN 11   //

#define lowVoltageSetting 693 // 11.1 lowV
// #define R_Batt 10000.0 resistor from battery to pin
// #define R_GND 4700.0    resistor from pin to gnd



SoftwareSerial XBee(XBEE_RX,XBEE_TX); // RX, TX
boolean UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON,L_TRIG,R_TRIG,JOYSTICK_BUTTON;
int verticalValue = 512, horizontalValue=512;
boolean JoystickBatteryGood;

struct operatingValues {
  int acc_calibration_value;                            //Enter the accelerometer calibration value (default: 1000)
  float pid_p_gain;                                       //Gain setting for the P-controller (default: 15)
  float pid_i_gain;                                      //Gain setting for the I-controller (default: 1.5)
  float pid_d_gain;                                       //Gain setting for the D-controller (default: 30)
  float turning_speed;                                    //Turning speed (default: 30)
  float max_target_speed;                                //Max target speed (default: 150)
};

operatingValues myOperatingValues;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Declaring global variables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean startProg;
boolean timingProblem;

int left_motor, throttle_left_motor, throttle_counter_left_motor, throttle_left_motor_memory;
int right_motor, throttle_right_motor, throttle_counter_right_motor, throttle_right_motor_memory;

long gyro_yaw_calibration_value, gyro_pitch_calibration_value;

unsigned long loop_timer;

float angle_gyro, angle_acc, angle, self_balance_pid_setpoint;
float pid_error_temp, pid_i_mem, pid_setpoint, gyro_input, pid_output, pid_last_d_error;
float pid_output_left, pid_output_right;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Setup basic functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  Serial.begin(9600);                                                       //Start the serial port at 9600 kbps
  Serial.println(F("Balancing_robot_Attempt3"));
  Serial.println(F("Last Modified Nov 7 2017"));
  
  Wire.begin();                                                             //Start the I2C bus as master
  TWBR = 12;                                                                //Set the I2C clock speed to 400kHz
  XBee.begin(9600);

pinMode(CALIB_LED,OUTPUT);   //yellow
pinMode(P_GAIN,OUTPUT);   //blue
pinMode(I_GAIN,OUTPUT);   //green
pinMode(D_GAIN,OUTPUT);   //red
pinMode(EDIT_MODE,OUTPUT);   //red
pinMode(ENABLE,OUTPUT);   //red
pinMode(PUSH_BUTTON,INPUT);   //SW1

pinMode(LEFT_DIR,OUTPUT);   //direction
pinMode(LEFT_STP,OUTPUT);   //step
pinMode(RIGHT_DIR,OUTPUT);   //direction
pinMode(RIGHT_STP,OUTPUT);   //step

digitalWrite(ENABLE,HIGH); // stop current through steppers



if (digitalRead(PUSH_BUTTON))
{
    defaultOperatingValues();
    digitalWrite(P_GAIN,HIGH);   //blue
    digitalWrite(I_GAIN,HIGH);   //green
    digitalWrite(D_GAIN,HIGH);   //red
}



loadOperatingValues();


setupTimer();
setGyroRegisters();
GyroCalibration();

/*
while(1)
{
  Serial.println(getBalanceValue());
  delay(100);
}
*/




  loop_timer = micros() + 4000;                                             //Set the loop_timer variable at the next end loop time

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Main program loop
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){

 getLatestXBeeData();

 checkBatteryVoltage(analogRead(VBATT));
 angleCalculation();
 PIDCalculations();
 controlCalculation();
 MotorPulseCalc();
 
  if(!startProg){
    digitalWrite(EDIT_MODE, !digitalRead(EDIT_MODE)); 
    delay(50);
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Loop time timer
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //The angle calculations are tuned for a loop time of 4 milliseconds. To make sure every loop is exactly 4 milliseconds a wait loop
  //is created by setting the loop_timer variable to +4000 microseconds every loop.
 
 // while(loop_timer > micros());
 // loop_timer += 4000;

   timingProblem = true;
  while(loop_timer > micros()){
    timingProblem = false; // if the other code takes too long, we never enter the loop.
  }
  loop_timer = micros() + 4000;
    digitalWrite(I_GAIN,!timingProblem);
    digitalWrite(P_GAIN,timingProblem);
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Interrupt routine  TIMER2_COMPA_vect
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(TIMER2_COMPA_vect){
  //Left motor pulse calculations
  throttle_counter_left_motor ++;                                           //Increase the throttle_counter_left_motor variable by 1 every time this routine is executed
  if(throttle_counter_left_motor > throttle_left_motor_memory){             //If the number of loops is larger then the throttle_left_motor_memory variable
    throttle_counter_left_motor = 0;                                        //Reset the throttle_counter_left_motor variable
    throttle_left_motor_memory = throttle_left_motor;                       //Load the next throttle_left_motor variable
    if(throttle_left_motor_memory < 0){                                     //If the throttle_left_motor_memory is negative
      PORTD &= 0b11101111;                                                  //Set output 4 low to reverse the direction of the stepper controller
      throttle_left_motor_memory *= -1;                                     //Invert the throttle_left_motor_memory variable
    }
    else PORTD |= 0b00010000;                                               //Set output 4 high for a forward direction of the stepper motor
  }
  else if(throttle_counter_left_motor == 1)PORTD |= 0b00100000;             //Set output 5 high to create a pulse for the stepper controller
  else if(throttle_counter_left_motor == 2)PORTD &= 0b11011111;             //Set output 5 low because the pulse only has to last for 20us 
  
  //right motor pulse calculations
  throttle_counter_right_motor ++;                                          //Increase the throttle_counter_right_motor variable by 1 every time the routine is executed
  if(throttle_counter_right_motor > throttle_right_motor_memory){           //If the number of loops is larger then the throttle_right_motor_memory variable
    throttle_counter_right_motor = 0;                                       //Reset the throttle_counter_right_motor variable
    throttle_right_motor_memory = throttle_right_motor;                     //Load the next throttle_right_motor variable
    if(throttle_right_motor_memory < 0){                                    //If the throttle_right_motor_memory is negative
       PORTD &= 0b10111111;                                                  //Set output 6 low to reverse the direction of the stepper controller
      throttle_right_motor_memory *= -1;                                    //Invert the throttle_right_motor_memory variable
    }
    else PORTD |= 0b01000000;                                               //Set output 6 high for a forward direction of the stepper motor
  }
  else if(throttle_counter_right_motor == 1)PORTD |= 0b10000000;            //Set output 7 high to create a pulse for the stepper controller
  else if(throttle_counter_right_motor == 2)PORTD &= 0b01111111;            //Set output 7 low because the pulse only has to last for 20us
}


























