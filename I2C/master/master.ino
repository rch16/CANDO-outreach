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
int slave_id[max_size]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int Mot_cycle[max_size] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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
  for(int i = 0; i < nbMotor; i++){
    send_data(go);
  }
  
  delay(10000);
  
  for(int i = 1; i < nbMotor; i++){
    send_data(stop);
  }

  delay(10000);
}

// Serial data coming from the PC interfae currently comes in the form of a single integer
// This represents the amount of terminals connected to the network
void serialEvent() {
  // nbMotor = Serial.read(); // Number of terminals >> Number of motors

  }

void send_data(bool sendData){
  Wire.beginTransmission(slave); // transmit to device #slave
  Wire.write(sendData);          // sends data 
  Wire.endTransmission();        // end transmission
  }


