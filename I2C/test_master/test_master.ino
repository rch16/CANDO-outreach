// test master i2c

#include <Wire.h>

void setup()
{
  Wire.begin(); 
  Serial.begin(9600);
}

void loop()
{
  Wire.beginTransmission(1);
  Wire.write('H');
  Wire.endTransmission();
 
  delay(500);
  
  Wire.beginTransmission(1);
  Wire.write('L');
  Wire.endTransmission();
 
  delay(500);
  
  Wire.beginTransmission(2);
  Wire.write('H');
  Wire.endTransmission();
 
  delay(500);
  
  Wire.beginTransmission(2);
  Wire.write('L');
  Wire.endTransmission();
 
  delay(500);
}
