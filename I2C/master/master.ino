// master arduino motor control
// SDA = A4
// SCL = A5
// Common GND

#include <Wire.h>

const int ledPin = 13;
const int slaveNum = 2; // number of slaves

int x = 0;
int y = 0;
int Serial_data;
int ready = 0; // ready to start or not?
int slaveID[slaveNum] = {1,2}; // IDs of Slave Arduinos -> change this to user input?

void setup() {
  // Start the I2C Bus as Master - address optional
  Wire.begin();
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

}
void loop() {
  if(ready == 1){
    for(int i = 0; i < slaveNum; i++){
      send_data(1); // go
    } 
  }
  else{ // if ready == 0
    for(int i = 0; i < slaveNum; i++){
      send_data(0); // stop
    }
  }
}

// Serial data coming from the PC interfae currently comes in the form of a single integer
// This represents the amount of terminals connected to the network
void serialEvent() {
  ready = Serial.read(); // Ready to start or not? 0 = NO, 1 = YES
  if(ready == 1){
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
  }
  }

void send_data(int sendData){
  for(int i = 0; i < slaveNum; i++){
    Wire.beginTransmission(slaveID[i]); // connect to device
    Wire.write(sendData); // send data
    Wire.endTransmission(); // end transmission
  }
}
