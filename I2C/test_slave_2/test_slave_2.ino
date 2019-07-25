// test slave i2c - arduino 2

#include <Wire.h>

const byte slaveId = 2;

void setup()
{
  // begin i2c communication at relevant address
  Wire.begin(slaveId); 
  // if data received, call receiveEvent function
  Wire.onReceive(receiveEvent);
 
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);    
}

void loop()
{
}

void receiveEvent(int howMany)
{
  // communication received
  char inChar;
  
  while(Wire.available() > 0)
  {
    // read data
    inChar = Wire.read();  
    
    if (inChar == 'H')
    {
      // LED high
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else if (inChar == 'L')
    {
      // LED low
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}
