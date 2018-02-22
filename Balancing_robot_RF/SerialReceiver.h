
#include "Arduino.h"
#include <SoftwareSerial.h>

#ifndef SerialReceiver_H
#define SerialReceiver_H

class SerialReceiver {
  public:
    SerialReceiver(SoftwareSerial * aSerial);
    void getLatestData();
    void startReceiver(uint32_t baud);

    boolean UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, L_TRIG, R_TRIG, JOYSTICK_BUTTON;
    int verticalValue = 512, horizontalValue = 512;
    boolean JoystickBatteryGood;
    unsigned long lastRecieved;

  private:
    SoftwareSerial * mySerial;
    void FlushData();
    void FlushUnilLatest();
    void resetRemoteInput();
    void processTheByte(byte aByte);
    int unpackJoystick(byte JSVal);
    boolean isClear(byte b, int pos);
    boolean isSet(byte b, int pos);
    byte customByte(boolean a, boolean b, boolean c, boolean d, boolean e, boolean f, boolean g, boolean h);
};

#endif

