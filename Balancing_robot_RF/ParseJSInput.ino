// two different flavors of bytes are sent. 

// first kind starts has leftmost bit as zero, and 7 bits corresponding to buttons
/* send byte of the form
 * (left to right 
 *  
 *  7: always 0
 *  6: Joystick button state
 *  5: Left Trigger State
 *  4: Right trigger state
 *  3: up button state
 *  2: down button state
 *  1: left button state
 *  0: right button state
 */

// second kind has vertical and horizontal values in 3 bits each, and battery good/bad in one bit.
/* send byte of the form
 * (left to right 
 *  
 *  7: always 1
 *  6: battery state (1 for good, 0 for lower than 75%)
 *  5: Horizontal value
 *  4: Horizontal value
 *  3: Horizontal value
 *  2: Vertical Value
 *  1: Vertical Value
 *  0: Vertical Value
 */


void getLatestXBeeData()
{
  static unsigned long lastRecieved;
  XBeeFlushUnilLatest(); // get rid of all the data except the most recent two bytes.
// fast projects may have an empty serial, in which case they do nothing
// fast projects may also encounter a single byte, in which case they parse it (they will get the complimentary byte in ~ 10 millisends
// a slow project has the latest two bytes, which should be no older than 30 milliseconds
  for(int x = 0;x<2;x++) 
  {
      if (XBee.available() > 0)
      {
        byte incomingData;
        incomingData = XBee.read();
        processTheByte(incomingData); // send byte to be unpacked
        lastRecieved = millis(); // Reset all to default if we haven't heard from the remote in a while
      }
  }
  
    if (lastRecieved + 250 < millis()){
      resetRemoteInput();
    }
}


void XBeeFlush(){
  while(XBee.available() > 0) {
    char t = XBee.read();
  }
}

void XBeeFlushUnilLatest(){
// flush all the older data away. Keep most recent two bytes
  while(XBee.available() > 2) {
    char t = XBee.read();
  }
}

void resetRemoteInput()
{
      UP_BUTTON = false,DOWN_BUTTON = false,LEFT_BUTTON = false,RIGHT_BUTTON = false;
      L_TRIG = false, R_TRIG = false,JOYSTICK_BUTTON = false;
      verticalValue = 512, horizontalValue = 512;
}


void processTheByte(byte aByte)
{
  // check each bit of the incoming byte, and unpack the joystick values.
  
  if (isClear(aByte,7)){            // unpacking button values
    JOYSTICK_BUTTON = isSet(aByte,6);
    L_TRIG = isSet(aByte,5);
    R_TRIG = isSet(aByte,4);
    UP_BUTTON = isSet(aByte,3);
    DOWN_BUTTON = isSet(aByte,2);
    LEFT_BUTTON = isSet(aByte,1);
    RIGHT_BUTTON = isSet(aByte,0);
   }
   
  if (isSet(aByte,7)){   // unpack joystick & batteries

    byte H_val = customByte(false,false,false,false,false,isSet(aByte,5),isSet(aByte,4),isSet(aByte,3));
    byte V_val = customByte(false,false,false,false,false,isSet(aByte,2),isSet(aByte,1),isSet(aByte,0));
    JoystickBatteryGood = isSet(aByte,6);
    horizontalValue = unpackJoystick(H_val);
    verticalValue = unpackJoystick(V_val);
  }

}

int unpackJoystick(byte JSVal) // map joystick back to 0 to 1023 range
{
  if (JSVal==1) return 0;
  if (JSVal==2) return 220;
  if (JSVal==3) return 365;
  if (JSVal==4)  return 512;
  if (JSVal==5) return 657;
  if (JSVal==6) return 803;
  //if (JSVal==7)
  return 1023;

}


