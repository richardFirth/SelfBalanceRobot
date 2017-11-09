
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
      SetAllLEDS(1,0,1,0,1); 
      delay(250);
      SetAllLEDS(0,1,0,1,0);   
      delay(250);
   }
}
