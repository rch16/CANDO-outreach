// slave arduino motor control

// I2C communication sends:
// Number of Steps = 6240
// Max speed = 600
// Iterations = 10

#include <Wire.h>

// - - - - - - - - - PINS - - - - - - - - - 
byte directionPin = 9;
byte stepPin = 8;
byte ledPin = 13;

// - - - - - - - - - SLAVE DATA - - - - - - - - - 
// Slave address has to match up to where the Master is sending information
const byte slaveID = 1; // change depending on which arduino you're sending code to

// - - - - - - - - - MOTOR INITIALISATION - - - - - - - - - 

const int exponential_steps = 50;     //Number of steps in the exponential stages
const int exponential_increment = 10;   //How much the speed will have changed by after exponential_steps
const int minspeed = 2500; //1000; // delay, not speed
int maxspeed = 1000; //300;
int numberOfSteps = 1125; //ONLY WORKS WITH NBOFSTEPS > 1524 -> incorrect value?
int iterations = 1;
int motorSpeed = 0;

// - - - - - - - - - MOTOR OPERATION ARRAYS - - - - - - - - - 
int Sa[exponential_steps];   //Array starting at low speed
int Sb[exponential_steps];   //Array finishing at high speed
int Sc[2*exponential_steps]; //Extended array starting at low speed
int temp[exponential_steps]; //Temp array
int time_array[6]; //tn determine the steps at which we switch between linear/exponential growth

// - - - - - - - - - I2C COMMUNICATION - - - - - - - - - 
bool go = false; 
int input = false;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void setup() 
{ 
  Serial.begin(9600);

  Wire.begin(slaveID); // begin i2c communication at relevant address
  Wire.onReceive(receiveEvent); // if data received, call receiveEvent function

  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  
  digitalWrite(LED_BUILTIN,LOW); 
  digitalWrite(directionPin, HIGH); // start with direction pin high
  // -> Pulley goes down first

  create_arrays(); // creating arrays
  //time_array_mod(time_array); // assigning time array
  time_array_mod(time_array, exponential_increment, maxspeed, exponential_steps, minspeed, numberOfSteps);
}

void loop() { 
  int d = digitalRead(directionPin);
  
  /*if(go){
    decode_transmission();
    go = false;
    
    for(int i = 0; i < iterations; i++){
      change_direction(d);
      motorSpeed = minspeed;
      for (int i = 0; i < numberOfSteps; i++){
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(motorSpeed + 4);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(motorSpeed);
        motorSpeed = step_interval(i, time_array);
        //Serial.println(motorSpeed);
        }
      delayMicroseconds(100);
    }
  }*/
  if (go){
    decode_transmission();
    go =  false;

    for(int i = 0; i < iterations; i++){
      digitalWrite(directionPin, HIGH);
      motorSpeed = minspeed;
      for (int i = 0; i < numberOfSteps; i++){
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(motorSpeed + 4);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(motorSpeed);
        motorSpeed = step_interval(i, time_array);
        //Serial.println(motorSpeed);
        }
      delayMicroseconds(4);
    
      digitalWrite(directionPin, LOW);
      motorSpeed = minspeed;
      for (int i = 0; i < numberOfSteps; i++){
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(motorSpeed + 4);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(motorSpeed);
        motorSpeed = step_interval(i, time_array);
      }
    }
  digitalWrite(LED_BUILTIN, LOW); // motor control complete
  }
}

void decode_transmission(){
  // decode the data sent via I2C using bit shifts
    Serial.println("Reading from I2C...");
    numberOfSteps = Wire.read();
    numberOfSteps += Wire.read() << 4;
    numberOfSteps += Wire.read() << 8;
    numberOfSteps += Wire.read() << 12;
    Serial.print("Number of Steps = ");
    Serial.println(numberOfSteps);

    maxspeed = Wire.read()*10;
    Serial.print("Max speed = ");
    Serial.println(maxspeed);

    iterations = Wire.read();
    Serial.print("Iterations = ");
    Serial.println(iterations);

    time_array_mod(time_array, exponential_increment, maxspeed, exponential_steps, minspeed, numberOfSteps);
    exp_array_mod(exponential_steps, maxspeed, Sb);
}

void create_arrays(){
  //USING L/1+exp(-k(x-mid))
  // L = curve's maximum value = 500
  // k = steepness of curve = 0.1
  int mid = 20; // mid-point x-value of curve
  for(int i = 0; i < exponential_steps; i++){
    Sa[i] = round(-500/(1+exp(-0.1*((i+1)-mid))) + minspeed);
    //Serial.println(Sa[i]);
    Sb[i] = round(-500/(1+exp(-0.1*(160-(exponential_steps-i) - 130))) + (maxspeed+480));
    //Serial.println(Sb[i]);
    temp[i] = Sa[i] - 411;
    }
  for(int j = 0; j < exponential_steps; j++){
    Sc[j] = Sa[j];
  }
  for(int k = exponential_steps; k < 2*exponential_steps; k++){
    Sc[k] = temp[k - exponential_steps];
  }
}

void change_direction(int d){
  if(d == 0){
    digitalWrite(directionPin, HIGH);
  }
  else{
    digitalWrite(directionPin, LOW);
  }
}
//Sets speed depending on step interval in which we are
int step_interval(int i, int t[]){
  if (i < t[0]){    //1st exponential (slow) increase from minspeed
    motorSpeed = Sa[i];
    //Serial.print("one = ");
  }
  else if(i >= t[0] && i < t[1]){   //linear increase y=x
    motorSpeed = 2024 - (i-t[0]);
    //Serial.print("two = ");
  }
  else if(i >= t[1] && i < t[2]){   //2nd exp increase until maxspeed
    motorSpeed = Sb[i-t[1]];
    //Serial.print("three = "); 
  }
  else if(i >= t[2] && i < t[3]){   //continue at maxspeed until motor needs to slow down
    //motorSpeed = maxspeed;
    //Serial.print("four = ");
  }
  else if(i > t[3] && i < t[4]){    //1st exponential decay from maxspeed
    motorSpeed = Sb[(exponential_steps-1)-(i-t[3])];
    //Serial.print("five = ");
  }
  else if(i >= t[4] && i < t[5]){   //linear decrease
    motorSpeed = 1420 + (i-t[4]); 
    //Serial.print("six = ");
  }
  else if(i >= t[5]){   //2nd exponential decay until minspeed
    motorSpeed = Sc[(2*exponential_steps-1)-(i-t[5])] + 76;
    //Serial.print("seven = ");
  }
  //Serial.println(motorSpeed);
  return motorSpeed;
 }

void time_array_mod(int t[], int exponential_increment, int maxspeed, int exponential_steps, int minspeed, int numberOfSteps){
  t[0] = exponential_steps;
  t[1] = exponential_steps + (minspeed-maxspeed) - 2*exponential_increment;
  t[2] = time_array[1] + exponential_steps;
  t[5] = numberOfSteps - exponential_steps;
  t[4] = t[5] - (minspeed-maxspeed - 2*exponential_increment) - 1;
  t[3] = t[4] - exponential_steps;
  // time intervals for different stages in the acceleration/deceleration curve
  /*t[0] = exponential_steps;
  t[1] = exponential_steps + minspeed/4 - 2*exponential_increment; 
  t[2] = t[1] + exponential_steps;
  t[3] = t[2];
  t[4] = t[3] + exponential_steps;
  t[5] = numberOfSteps - 2*exponential_steps;*/
}

 void exp_array_mod(int exponential_steps, int maxspeed, int Sb[]){
  // modifiy the exponential motion array
  for(int i = 0; i < exponential_steps; i++){
    Sb[i] = round(-500/(1+exp(-0.1*(160-(exponential_steps-i) - 80))) + (500+maxspeed));
    }
  }

void receiveEvent(int bytes){
  // communication received
  digitalWrite(LED_BUILTIN, HIGH);// LED high
  go = true;
}
