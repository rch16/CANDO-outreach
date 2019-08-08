// This file is used to measure the time it takes the code to complete a full iteration (up and down)
// - without any I2C
// - to calculate the range of delays at various numbers of steps and max speeds
// - a delay value from the middle of the results will be chosen as a reference delay
// - harmonic delays calculated from here
// - using harmonics will make the various motors naturally synchronise together after some time

// - - - - - - - - - PINS - - - - - - - - - 
byte directionPin = 9;
byte stepPin = 8;
byte ledPin = 13;

// - - - - - - - - - MOTOR INITIALISATION - - - - - - - - - 

const int exponential_steps = 50;     //Number of steps in the exponential stages
const int exponential_increment = 10;   //How much the speed will have changed by after exponential_steps
const int minspeed = 2500; //1000; // delay, not speed
int iterations = 1;
int motorSpeed = 0;

// - - - - - - - - - MOTOR OPERATION ARRAYS - - - - - - - - - 
int Sa[exponential_steps];   //Array starting at low speed
int Sb[exponential_steps];   //Array finishing at high speed
int Sc[2*exponential_steps]; //Extended array starting at low speed
int temp[exponential_steps]; //Temp array
int time_array[6]; //tn determine the steps at which we switch between linear/exponential growth

// - - - - - - - - - TESTING ITERATION TIMES - - - - - - - - - 
int time1, time2;
unsigned int interval;
int numberOfSteps = 400;
//int numberOfSteps = 800;
//int numberOfSteps = 1200;
int maxspeed = 300;
//int maxspeed = 400;
//int maxspeed = 500;
//int maxspeed = 600;
//int maxspeed = 700;
//int maxspeed = 800;
//int maxspeed = 900;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void setup() 
{ 
  Serial.begin(9600);

  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  
  digitalWrite(LED_BUILTIN,LOW); 
  // -> Pulley goes down first

  create_arrays(); // creating arrays
  time_array_mod(time_array); // assigning time array
}

void loop() { 
    time1 = millis();
    digitalWrite(LED_BUILTIN, HIGH); // motion begins
    // - - - - - - move T-Shirt down - - - - - - 
    digitalWrite(directionPin, HIGH);
    for (int i = 0; i < numberOfSteps; i++){
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(motorSpeed + 4);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(motorSpeed);
        motorSpeed = step_interval(i, time_array);
    }
    // - - - - - - move T-Shirt up - - - - - - 
    digitalWrite(directionPin, LOW); // change direction of motion
    for (int i = 0; i < numberOfSteps; i++){
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(motorSpeed + 4);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(motorSpeed);
        motorSpeed = step_interval(i, time_array);
    }

    digitalWrite(LED_BUILTIN, LOW); // full iteration complete
    time2 = millis();
    interval = time2 - time1;
    Serial.println(interval);
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

void time_array_mod(int t[]){
  // time intervals for different stages in the acceleration/deceleration curve
  t[0] = exponential_steps;
  t[1] = exponential_steps + minspeed/4 - 2*exponential_increment; 
  t[2] = t[1] + exponential_steps;
  t[3] = t[2];
  t[4] = t[3] + exponential_steps;
  t[5] = numberOfSteps - 2*exponential_steps;
}

 void exp_array_mod(int exponential_steps, int maxspeed, int Sb[]){
  // modifiy the exponential motion array
  for(int i = 0; i < exponential_steps; i++){
    Sb[i] = round(-500/(1+exp(-0.1*(160-(exponential_steps-i) - 80))) + (500+maxspeed));
    }
  }

