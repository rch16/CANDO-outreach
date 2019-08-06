// master arduino motor control
// SDA = A4
// SCL = A5
// Common GND

#include <Wire.h>

// - - - - - - - - - PIN SETUP - - - - - - - - - - - - 
const int ledPin = 13;

// - - - - - - - - - SYNCHRONISATION - - - - - - - - - 
// array containing the harmonics of a reference signal at 16,000ms:
const unsigned int harmonicDelays[9] = {6400, 9600, 12800, 16000, 19200, 22400, 25600, 28800, 32000}; // assigned randomly to every motor that isn't the reference motor
unsigned int stepsNum; // found by rearranging delay equations
unsigned int val;
int iteration = 5; // iteration for each harmonic before synchronisation = 5 
int x = 0;
int y = 0;

// - - - - - - - - - SLAVE DATA - - - - - - - - - - - -
const int slaveNum = 2; // number of slaves
int slaveID[slaveNum] = {1,2}; // IDs of Slave Arduinos

// - - - - - - - - - MOTOR INITIATION - - - - - - - - - 
const int maxSize = 10;
int ready = 0; // ready to start?
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

// Serial data coming from the PC interfae currently comes in the form of a single integer
// This represents the amount of terminals connected to the network
void serialEvent() {
  ready = Serial.read(); // Ready to start or not? 0 = NO, 1 = YES
  if(ready == 1){
    digitalWrite(LED_BUILTIN, HIGH);
    random_operation();
    for(int i = 0; i < slaveNum; i++){
      // send respective data to each slave
      send_data(motorData[i], slaveID[i]); // go
    } 
    delay(5000);
    synchronised_operation();
    for(int i = 0; i < slaveNum; i++){
      // send respective data to each slave
      send_data(motorData[i], slaveID[i]); // go
    } 
    // operation complete
    digitalWrite(LED_BUILTIN, LOW)
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void send_data(byte data, int slave){
  Wire.beginTransmission(slave); // connect to device
  Wire.write(data); // send data
  Wire.endTransmission(); // end transmission
}

// Structuring and transmission of data to the slaves for synchronised operation
void synchronised_operation(){
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
  
  val = round((harmonicDelays[3] - 747.0)/2.4445);

  // sending to motor 0 -> the reference motor
  // number of steps is sent using four bytes of data, seperated with bit shifts
  motorData[0][0] = (val & 0x000F);
  motorData[0][1] = (val & 0x00F0) >> 4; 
  motorData[0][2] = (val & 0x0F00) >> 8;
  motorData[0][3] = (val & 0xF000) >> 12;
  motorData[0][4] = 60;
  motorData[0][5] = 10;
  // motorData[i][6] is the 6 byte data sent to motor i 

  for(int i = 1; i < slaveNum; i++){

    y = random(3, 10); // randomly assign a number between 3 & 9 inclusive
      switch(y){ // what happens if each value is selected

      case 3: // if y = 3
        x = random(0, 3); // randomly assign one of x = 0,1,2
        switch(x){ 
          // WHAT is motorCycle[]?
          case 0: motorCycle[i] = 2; break;
          case 1: motorCycle[i] = 3; break;
          case 2: motorCycle[i] = 4; break;
          }

        val = round((harmonicDelays[x] - 2151.5)/1.2373);
        // format val to be transmitted using bit shift 
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        // what is bytes 4 & 5?
        motorData[i][4] = y*10;
        motorData[i][5] = 5;
        break;
      
      case 4: // if y = 4
        x = random(0, 4);
        switch(x){
          case 0: Mot_cycle[i] = 2; break;
          case 1: Mot_cycle[i] = 3; break;
          case 2: Mot_cycle[i] = 4; break;
          case 3: Mot_cycle[i] = 5; break;
          }
        val = round((harmonicDelays[x] - 1602.5)/1.6398);
        // format val to be transmitted using bit shift 
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        // what is bytes 4 & 5?
        motorData[i][4] = y*10;
        motorData[i][5] = 5;
        break;
      
      case 5:
        x = random(0, 5);
        switch(x){
          case 0: Mot_cycle[i] = 2; break;
          case 1: Mot_cycle[i] = 3; break;
          case 2: Mot_cycle[i] = 4; break;
          case 3: Mot_cycle[i] = 5; break;
          case 4: Mot_cycle[i] = 6; break;
          }
        val = round((harmonicDelays[x] - 1133.5)/2.0423);
        // format val to be transmitted using bit shift 
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        // what is bytes 4 & 5?
        motorData[i][4] = y*10;
        motorData[i][5] = 5;
        break;
      
      case 6:
        x = random(0, 6);
        switch(x){
          case 0: Mot_cycle[i] = 2; break;
          case 1: Mot_cycle[i] = 3; break;
          case 2: Mot_cycle[i] = 4; break;
          case 3: Mot_cycle[i] = 5; break;
          case 4: Mot_cycle[i] = 6; break;
          case 5: Mot_cycle[i] = 7; break;
          }
        val = round((harmonicDelays[x] - 747.0)/2.4445);
        // format val to be transmitted using bit shift 
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        // what is bytes 4 & 5?
        motorData[i][4] = y*10;
        motorData[i][5] = 5;
        break;
      
      case 7:
        x = random(0, 8);
        switch(x){
          case 0: Mot_cycle[i] = 2; break;
          case 1: Mot_cycle[i] = 3; break;
          case 2: Mot_cycle[i] = 4; break;
          case 3: Mot_cycle[i] = 5; break;
          case 4: Mot_cycle[i] = 6; break;
          case 5: Mot_cycle[i] = 7; break;
          case 6: Mot_cycle[i] = 8; break;
          case 7: Mot_cycle[i] = 9; break;
          }
        val = round((harmonicDelays[x] - 440.0)/2.847);
        // format val to be transmitted using bit shift 
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        // what is bytes 4 & 5?
        motorData[i][4] = y*10;
        motorData[i][5] = 5;
        break;
      
      case 8:
        x = random(1, 9);
        switch(x){
          case 1: Mot_cycle[i] = 3; break;
          case 2: Mot_cycle[i] = 4; break;
          case 3: Mot_cycle[i] = 5; break;
          case 4: Mot_cycle[i] = 6; break;
          case 5: Mot_cycle[i] = 7; break;
          case 6: Mot_cycle[i] = 8; break;
          case 7: Mot_cycle[i] = 9; break;
          case 8: Mot_cycle[i] = 4; break;
          }
        bigval = round((dels[x] - 213.5)/3.2493);
        // format val to be transmitted using bit shift 
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        // what is bytes 4 & 5?
        motorData[i][4] = y*10;
        motorData[i][5] = 5;
        break;
      
      case 9:
        x = random(1, 9);
        switch(x){
          case 1: Mot_cycle[i] = 3; break;
          case 2: Mot_cycle[i] = 4; break;
          case 3: Mot_cycle[i] = 5; break;
          case 4: Mot_cycle[i] = 6; break;
          case 5: Mot_cycle[i] = 7; break;
          case 6: Mot_cycle[i] = 8; break;
          case 7: Mot_cycle[i] = 9; break;
          case 8: Mot_cycle[i] = 4; break;
          }
        bigval = round((dels[x] - 66.5)/3.6518);
        // format val to be transmitted using bit shift 
        motorData[i][0] = (val & 0x000F);
        motorData[i][1] = (val & 0x00F0) >> 4;
        motorData[i][2] = (val & 0x0F00) >> 8;
        motorData[i][3] = (val & 0xF000) >> 12;
        // what is bytes 4 & 5?
        motorData[i][4] = y*10;
        motorData[i][5] = 5;
        break;
        }
}
}
