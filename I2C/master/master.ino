// master arduino motor control
// SDA = A4
// SCL = A5
// Common GND

#include <Wire.h>

bool go = true;
bool stop = false;

//Array with percentage delay values dels[3] = 16000 = 100%
const unsigned int dels[9] = {6400, 9600, 12800, 16000, 19200, 22400, 25600, 28800, 32000};

unsigned int bigval;

const int max_size = 10;
const int ledPin = 13;

int x = 0;
int y = 0;
int Serial_data;
int nbMotor = 2; // number of motors
int ready = 0; // ready to start or not?
int slaveID = 1;

byte stop_data[6] = {0, 0, 0, 0, 0, 0};
byte Mot_data[max_size][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
// Structure of Mot_data[i][j]:
//i is the motor number, 0 being reference
//j is each data type: 0 to 3 are the nbOfSteps, 4 is maxspeed, 5 is iterations

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
  Wire.beginTransmission(slaveID); // transmit to device #slave
  Wire.write(sendData);          // sends data 
  Wire.endTransmission();        // end transmission
  }
