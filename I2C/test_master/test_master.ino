// test master i2c
// working on: serial control of starting i2c communication

#include <Wire.h>

int ready = 0;

void setup()
{
  // begin i2c communication - address optional for master
  Wire.begin(); 
  // open a serial channel
  Serial.begin(9600);
}

void loop()
{
  Serial.println(ready);
  while(ready == 1){
    Serial.println("Go!");
    // transmit to slave 1, H = HIGH
    Wire.beginTransmission(1);
    Wire.write('H');
    Wire.endTransmission();
  
    delay(500);
    
    // transmit to slave 1, L = LOW
    Wire.beginTransmission(1);
    Wire.write('L');
    Wire.endTransmission();
  
    delay(500);
    
    // transmit to slave 2, H = HIGH
    Wire.beginTransmission(2);
    Wire.write('H');
    Wire.endTransmission();
  
    delay(500);
    
    // transmit to slave 2, L = LOW
    Wire.beginTransmission(2);
    Wire.write('L');
    Wire.endTransmission();
  
    delay(500);
  }
}

// Serial data coming from the PC interfae currently comes in the form of a single integer
// This represents the amount of terminals connected to the network
void serialEvent() {
  ready = Serial.read(); // Ready to start or not? 0 = NO, 1 = YES
  }
