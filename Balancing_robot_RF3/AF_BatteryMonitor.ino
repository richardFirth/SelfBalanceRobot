
void checkBatteryVoltage(int rawPinRead)
{
  if ((rawPinRead < lowVoltageSetting) &&(rawPinRead  >425))
  {
    digitalWrite(ENABLE,HIGH);
    lowVoltageAlarm();
  }
}

void lowVoltageAlarm()
{
   while(1){
      digitalWrite(P_GAIN, HIGH);
      digitalWrite(I_GAIN, LOW);
      digitalWrite(D_GAIN, HIGH); 
      delay(250);
      digitalWrite(P_GAIN, LOW);
      digitalWrite(I_GAIN, HIGH);
      digitalWrite(D_GAIN, LOW);  
      delay(250);
   }
}
