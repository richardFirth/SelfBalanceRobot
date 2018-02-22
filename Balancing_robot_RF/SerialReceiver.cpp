
#include "Arduino.h"
#include "SerialReceiver.h"

SerialReceiver::SerialReceiver(SoftwareSerial * aSerial) {
  mySerial = aSerial;
}

void SerialReceiver::startReceiver(uint32_t baud) {
  mySerial->begin(baud);
}

void SerialReceiver::getLatestData()
{
  FlushUnilLatest(); // get rid of all the data except the most recent.

  for (int x = 0; x < 2; x++) // get two bytes and parse them
  {
    if (mySerial->available() > 0)
    {
      byte incomingData;
      incomingData = mySerial->read();
      processTheByte(incomingData);
    }
  }

  lastRecieved = millis(); // zero stuff out if we haven't heard from the remote in a while
  if (lastRecieved + 250 < millis()) {
    resetRemoteInput();
  }

}



void SerialReceiver::FlushData() {
  while (mySerial->available() > 0) {
    char t = mySerial->read();
  }
}

void SerialReceiver::FlushUnilLatest() {
  // flush all the older data away. keep most recent two bytes
  while (mySerial->available() > 2) {
    char t = mySerial->read();
  }
}

void SerialReceiver::resetRemoteInput()
{
  UP_BUTTON = false, DOWN_BUTTON = false, LEFT_BUTTON = false, RIGHT_BUTTON = false;
  L_TRIG = false, R_TRIG = false, JOYSTICK_BUTTON = false;
  verticalValue = 512, horizontalValue = 512;
}

void SerialReceiver::processTheByte(byte aByte)
{

  if (isClear(aByte, 7)) {
    JOYSTICK_BUTTON = isSet(aByte, 6);
    L_TRIG = isSet(aByte, 5);
    R_TRIG = isSet(aByte, 4);
    UP_BUTTON = isSet(aByte, 3);
    DOWN_BUTTON = isSet(aByte, 2);
    LEFT_BUTTON = isSet(aByte, 1);
    RIGHT_BUTTON = isSet(aByte, 0);
  }

  if (isSet(aByte, 7)) {

    byte H_val = customByte(false, false, false, false, false, isSet(aByte, 5), isSet(aByte, 4), isSet(aByte, 3));
    byte V_val = customByte(false, false, false, false, false, isSet(aByte, 2), isSet(aByte, 1), isSet(aByte, 0));
    JoystickBatteryGood = isSet(aByte, 6);
    horizontalValue = unpackJoystick(H_val);
    verticalValue = unpackJoystick(V_val);
  }

}

int SerialReceiver::unpackJoystick(byte JSVal)
{
  if (JSVal == 1) return 0;
  if (JSVal == 2) return 220;
  if (JSVal == 3) return 365;
  if (JSVal == 4)  return 512;
  if (JSVal == 5) return 657;
  if (JSVal == 6) return 803;
  //if (JSVal==7)
  return 1023;
}

boolean SerialReceiver::isClear(byte b, int pos) {
  return !isSet(b, pos);
}

boolean SerialReceiver::isSet(byte b, int pos) {
  if (pos < 0 || pos > 7)
    return false;
  return (b >> pos & 1) == 1;
}

byte SerialReceiver::customByte(boolean a, boolean b, boolean c, boolean d, boolean e, boolean f, boolean g, boolean h)
{
  byte myByte = 0;
  if (a) myByte |= (1 << 7);
  if (b) myByte |= (1 << 6);
  if (c) myByte |= (1 << 5);
  if (d) myByte |= (1 << 4);
  if (e) myByte |= (1 << 3);
  if (f) myByte |= (1 << 2);
  if (g) myByte |= (1 << 1);
  if (h) myByte |= (1 << 0);

  return myByte;
}
