// master arduino motor control
// SDA = A4
// SCL = A5
// Common GND

#include <Wire.h>

bool go = true;
bool stop = false;

const int ledPin = 13;

int x = 0;
int y = 0;
int Serial_data;
int nbMotor = 2; // number of motors
int ready = 0; // ready to start or not?
int slaveID[2] = {1,2}; // IDs of Slave Arduinos

void setup() {
  // Start the I2C Bus as Master - address optional
  Wire.begin();

  Serial.begin(9600);

  digitalWrite(ledPin, HIGH);

}
void loop() {
  if(ready == 1){
    for(int i = 0; i < nbMotor; i++){
      send_data(1);
    }
    
    delay(10000);
    
    for(int i = 1; i < nbMotor; i++){
      send_data(0);
    }

    delay(10000);
  }
}

// Serial data coming from the PC interfae currently comes in the form of a single integer
// This represents the amount of terminals connected to the network
void serialEvent() {
  ready = Serial.read(); // Ready to start or not? 0 = NO, 1 = YES
  }

void send_data(int sendData){
  for(int i = 0; i < slaveID.size(); i++){
    Wire.beginTransmission(slaveID[i]); // connect to device
    Wire.write(sendData); // send data
    Wire.endTransmission(); // end transmission
  }
}
