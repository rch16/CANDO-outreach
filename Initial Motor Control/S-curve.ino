// Revision control 
// i2c -> sooner rather than later
#include <Wire.h>

byte directionPin = 9;
byte stepPin = 8;
int numberOfSteps = 1125; //ONLY WORKS WITH NBOFSTEPS > 1524
int iterations=1;
const int arrsz = 50;     //Number of steps it takes for ∂f/∂x to reach 1 
const int exp_inc = 10;   //what speed value is reached after arrsz steps 
byte ledPin = 13;
const int minspeed = 2500;//1000; // delay, not speed
int maxspeed = 1000;//300;
int motorSpeed = 0;
int Sa[arrsz];   //Array starting at low speed
int Sb[arrsz];   //Array finishing at high speed
int Sc[2*arrsz]; //Extended array starting at low speed
int temp[arrsz]; //Temp array

//tn determine the steps at which we switch between linear/exponential growth
int time_array[6] = {0,0,0,0,0,0};
//t[0] = arrsz;
//t[1] = arrsz + (minspeed-maxspeed) - 2*exp_inc;
//t[2] = time_array[1] + arrsz;
//t[5] = numberOfSteps - arrsz;
//t[4] = t[5] - (minspeed-maxspeed - 2*exp_inc) - 1;
//t[3] = t[4] - arrsz;

  
void setup() 
{ 

  Serial.begin(9600);
  Serial.println("Starting Stepper");
  
  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);

  Serial.println("Creating arrays");
  createArrays();

  Serial.println("Assigning time array");
  time_array_mod(time_array);

  Serial.println("Start with direction pin high");
  digitalWrite(directionPin, HIGH);

  Serial.println("Start motor operation");
}

void loop() { 
  int d = digitalRead(directionPin);
  for(int i = 0; i < iterations; i++){
    changeDirection(d);
    motorSpeed = minspeed;
    for (int i = 0; i < numberOfSteps; i++){
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(motorSpeed + 4);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(motorSpeed);
      motorSpeed = stepInterval(i, time_array);
      //Serial.println(motorSpeed);
      }
    delayMicroseconds(100);
  }
}

void createArrays(){
  //USING L/1+exp(-k(x-mid))
  // L = curve's maximum value = 500
  // k = steepness of curve = 0.1
  int mid = 20; // mid-point x-value of curve
  for(int i = 0; i < arrsz; i++){
    Sa[i] = round(-500/(1+exp(-0.1*((i+1)-mid))) + minspeed);
    //Serial.println(Sa[i]);
    Sb[i] = round(-500/(1+exp(-0.1*(160-(arrsz-i) - 130))) + (maxspeed+480));
    //Serial.println(Sb[i]);
    temp[i] = Sa[i] - 411;
    }
  for(int j = 0; j < arrsz; j++){
    Sc[j] = Sa[j];
  }
  for(int k = arrsz; k < 2*arrsz; k++){
    Sc[k] = temp[k - arrsz];
  }
}

void changeDirection(int d){
  if(d == 0){
    digitalWrite(directionPin, HIGH);
  }
  else{
    digitalWrite(directionPin, LOW);
  }
}
//Sets speed depending on step interval in which we are
int stepInterval(int i, int t[]){
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
    motorSpeed = Sb[(arrsz-1)-(i-t[3])];
    //Serial.print("five = ");
  }
  else if(i >= t[4] && i < t[5]){   //linear decrease
    motorSpeed = 1420 + (i-t[4]); 
    //Serial.print("six = ");
  }
  else if(i >= t[5]){   //2nd exponential decay until minspeed
    motorSpeed = Sc[(2*arrsz-1)-(i-t[5])] + 76;
    //Serial.print("seven = ");
  }
  //Serial.println(motorSpeed);
  return motorSpeed;
 }

void time_array_mod(int t[]){
  t[0] = arrsz;
  t[1] = arrsz + minspeed/4 - 2*exp_inc; 
  t[2] = t[1] + arrsz;
  t[3] = t[2];
  t[4] = t[3] + arrsz;
  t[5] = numberOfSteps - 2*arrsz;
  for(int i = 0; i < 6; i++){
    Serial.print("t[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(t[i]);
  }
}
