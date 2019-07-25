// test slave i2c

#include <Wire.h>

const byte slaveId = 2;

void setup()
{
  Wire.begin(slaveId); 
  Wire.onReceive(receiveEvent);
 
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);    
}

void loop()
{
}

void receiveEvent(int howMany)
{
  char inChar;
  
  while(Wire.available() > 0)
  {
    inChar = Wire.read();  
    
    if (inChar == 'H')
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else if (inChar == 'L')
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}
