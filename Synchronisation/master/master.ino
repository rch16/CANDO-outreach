// master arduino motor control
// SDA = A4
// SCL = A5
// Common GND

#include <Wire.h>

// - - - - - - - - - PIN SETUP - - - - - - - - - - - - 
const int ledPin = 13;

// - - - - - - - - - SYNCHRONISATION - - - - - - - - - 
// array containing the harmonics of a reference signal at 16,000ms:
const unsigned int harmonicDelays[7] = {741,988,1235,1482,1729,1976,2223}; // assigned randomly to every motor that isn't the reference motor. n = 3 to n = 9
unsigned int stepsNum; // found by rearranging delay equations
unsigned int val;
int iteration = 5; // iteration for each harmonic before synchronisation = 5 
int x = 0;
int y = 0;

// - - - - - - - - - SLAVE DATA - - - - - - - - - - - -
// -> implement user defining slave number, then function to create an array {1,2,3,...n}, will define maxSize as well
const int slaveNum = 3; // number of slaves
int slaveID[slaveNum] = {1,2,3}; // IDs of Slave Arduinos

// - - - - - - - - - MOTOR INITIATION - - - - - - - - - 
const int maxSize = 10; // currently limits code to max 10 motors 
int ready = 0; // ready to start?
//Number of iterations done by reference when motor n will have synced up with it
//Array containing motion profile data of each motor
int motorCycle[maxSize] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte motorData[maxSize][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void setup() {
  // Start the I2C Bus as Master - address optional
  Wire.begin();
  // Setup the serial terminal
  Serial.begin(9600);
  // Use built in led for debugging
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

// main
void loop() {
  // good practice to keep this as empty as possible
}

// Serial data coming from the PC interface currently comes in the form of a single integer
// This represents the amount of terminals connected to the network
void serialEvent() {
  ready = Serial.read(); // Ready to start or not? 0 = NO, 1 = YES
  if(ready == 1){
    digitalWrite(LED_BUILTIN, HIGH);
    random_operation();
    for(int i = 0; i < slaveNum; i++){
      // send respective motion data to each 
      send_data(motorData[i], slaveID[i]); // go
    } 
    delay(5000);
    synchronised_operation();
    for(int i = 0; i < slaveNum; i++){
      // send respective data to each slave
      send_data(motorData[i], slaveID[i]); // go
    }
  }
  else{
    // do nothing, wait for the next time that ready = 1
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void send_data(byte data[], int slave){
  Wire.beginTransmission(slave); // connect to device
  Wire.write(data, 6); // send data
  Wire.endTransmission(); // end transmission
}

// Structuring and transmission of data to the slaves for synchronised operation
//The sync timing is used here to determine for how long each motor will have to move in sync with reference before motor 0 will complete 10 iterations
void synchronised_operation(){
  // Needs to be called after random operation
  // Otherwise reference motor will have motion values of 0 and hence so will all the motors 
   for(int i = 1; i < slaveNum; i++){
    motorData[i][5] = 10 - motorCycle[i];
     for(int j = 0; j < 5; j++){
       // assign all motors the same value as the reference motor (#0)
       motorData[i][j] = motorData[0][j];
       }
     }
   }

// Structuring and transmission of data to the slaves for random operation
void random_operation(){

  // Initialise the random number generator with a fairly random input
  // such as analogRead() on the unconnected A0 pin.
  randomSeed(analogRead(A0)); 
  
  // Assign reference delay
  val = round((harmonicDelays[2] - 120.67)/1.4563);

  // sending to motor 0 -> the reference motor
  // number of steps is sent using four bytes of data, seperated with bit shifts
  motorData[0][0] = (val & 0x000F);
  motorData[0][1] = (val & 0x00F0) >> 4; 
  motorData[0][2] = (val & 0x0F00) >> 8;
  motorData[0][3] = (val & 0xF000) >> 12;
  motorData[0][4] = 50; // chosen 500 as reference max speed
  motorData[0][5] = 10; // iterations
  // motorData[i][6] is the 6 byte data sent to motor i 

  for(int i = 1; i < slaveNum; i++){

    y = random(3, 10); // First randomly allocate a number between 3 and 9 -> translates to a maxspeed between 300 and 900
      switch(y){ // what happens if each value is selected
      case 3: // if max speed = 300
        // Then randomly allocate iteration delay value
        // Note that interval varies because for example, a maxspeed of 300 cant generate a delay greater than 1731
        // This translates to the array of a max of 1729, when n = 7
        x = random(0, 8);
        switch(x){ 
          case 0: motorCycle[i] = 2; break;
          case 1: motorCycle[i] = 3; break;
          case 2: motorCycle[i] = 4; break;
          case 3: motorCycle[i] = 5; break;
          case 4: motorCycle[i] = 6; break;
          case 5: motorCycle[i] = 7; break;
          case 6: motorCycle[i] = 8; break;
          case 7: motorCycle[i] = 9; break;
          }

        val = round((harmonicDelays[x] - 107.67)/1.32);
        // format val to be transmitted using bit shift 
        // number of steps, bitshifted:
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        motorData[i][4] = y*10; // max speed
        motorData[i][5] = 5; // 5 iterations
        break;
      
      case 4: // if max speed = 400
        x = random(0, 8);
        switch(x){ 
          case 0: motorCycle[i] = 2; break;
          case 1: motorCycle[i] = 3; break;
          case 2: motorCycle[i] = 4; break;
          case 3: motorCycle[i] = 5; break;
          case 4: motorCycle[i] = 6; break;
          case 5: motorCycle[i] = 7; break;
          case 6: motorCycle[i] = 8; break;
          case 7: motorCycle[i] = 9; break;
          }
        val = round((harmonicDelays[x] - 119.67)/1.3763);
        // number of steps: 
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        motorData[i][4] = y*10; // max speed
        motorData[i][5] = 5; // iterations
        break;
      
      case 5: // if max speed = 500
        x = random(0, 8);
        switch(x){ 
          case 0: motorCycle[i] = 2; break;
          case 1: motorCycle[i] = 3; break;
          case 2: motorCycle[i] = 4; break;
          case 3: motorCycle[i] = 5; break;
          case 4: motorCycle[i] = 6; break;
          case 5: motorCycle[i] = 7; break;
          case 6: motorCycle[i] = 8; break;
          case 7: motorCycle[i] = 9; break;
          }
        val = round((harmonicDelays[x] - 137.33)/1.3963);
        // number of steps:
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        motorData[i][4] = y*10; // max speed
        motorData[i][5] = 5; // iterations
        break;
      
      case 6: // if max speed = 600
        x = random(0, 8);
        switch(x){ 
          case 0: motorCycle[i] = 2; break;
          case 1: motorCycle[i] = 3; break;
          case 2: motorCycle[i] = 4; break;
          case 3: motorCycle[i] = 5; break;
          case 4: motorCycle[i] = 6; break;
          case 5: motorCycle[i] = 7; break;
          case 6: motorCycle[i] = 8; break;
          case 7: motorCycle[i] = 9; break;
          }
        val = round((harmonicDelays[x] - 120.67)/1.4563);
        // number of steps:
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        motorData[i][4] = y*10; // max speed
        motorData[i][5] = 5; // iterations
        break;
      
      case 7: // if max speed = 700
        x = random(0, 8);
        switch(x){ 
          case 0: motorCycle[i] = 2; break;
          case 1: motorCycle[i] = 3; break;
          case 2: motorCycle[i] = 4; break;
          case 3: motorCycle[i] = 5; break;
          case 4: motorCycle[i] = 6; break;
          case 5: motorCycle[i] = 7; break;
          case 6: motorCycle[i] = 8; break;
          case 7: motorCycle[i] = 9; break;
          }
        val = round((harmonicDelays[x] - 403.67)/1.5325);
        // number of steps:
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        motorData[i][4] = y*10; // max speed
        motorData[i][5] = 5; // iterations
        break;
      
      case 8: // if max speed = 800
        x = random(1, 9); // minimum speed higher than case 0 option
        switch(x){
          case 1: motorCycle[i] = 3; break;
          case 2: motorCycle[i] = 4; break;
          case 3: motorCycle[i] = 5; break;
          case 4: motorCycle[i] = 6; break;
          case 5: motorCycle[i] = 7; break;
          case 6: motorCycle[i] = 8; break;
          case 7: motorCycle[i] = 9; break;
          case 8: motorCycle[i] = 4; break;
          }
        val = round((harmonicDelays[x] - 94.0)/1.6363);
        // number of steps:
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        motorData[i][4] = y*10; // max speed
        motorData[i][5] = 5; // iterations
        break;
      
      case 9: // if max speed = 900
        x = random(1, 9);
        switch(x){
          case 1: motorCycle[i] = 3; break;
          case 2: motorCycle[i] = 4; break;
          case 3: motorCycle[i] = 5; break;
          case 4: motorCycle[i] = 6; break;
          case 5: motorCycle[i] = 7; break;
          case 6: motorCycle[i] = 8; break;
          case 7: motorCycle[i] = 9; break;
          case 8: motorCycle[i] = 4; break;
          }
        val = round((harmonicDelays[x] - 34.0)/1.825);
        // number of steps:
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        motorData[i][4] = y*10; // max speed
        motorData[i][5] = 5; // iterations
        break;
        }
    }
}
