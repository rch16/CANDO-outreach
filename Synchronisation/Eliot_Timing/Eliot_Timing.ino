byte directionPin = 9;
byte stepPin = 8;
const int numberOfSteps = 1200; //ONLY WORKS WITH NBOFSTEPS > 1124
const int arrsz = 41;     //Number of steps it takes for ∂f/∂x to reach 1 
const int exp_inc = 10;   //what speed value is reached after arrsz steps 
byte ledPin = 3;
const int minspeed = 1000;
const int maxspeed = 900;
int zoom = 0;
int Sa[arrsz];  //Array starting at low speed
int Sb[arrsz];  //Array finishing at high speed

//tn determine the steps at which we switch between linear/exponential growth
int t1 = arrsz;
int t2 = arrsz + (minspeed-maxspeed) - 2*exp_inc;
int t3 = t2 + arrsz;
int t6 = numberOfSteps - arrsz;
int t5 = t6 - (minspeed-maxspeed - 2*exp_inc) - 1;
int t4 = t5 - arrsz;
int illum;

void setup() 
{ 

  Serial.begin(9600);
  Serial.println("Starting Stepper");
  digitalWrite(ledPin, LOW);

  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  
//  Serial.println(t1);
//  Serial.println(t2);
//  Serial.println(t3);
//  Serial.println(t4);
//  Serial.println(t5);
//  Serial.println(t6);

  Serial.println("Creating arrays");
  //USING 500/1+exp(-0.1(x-80))
  for(int i = 0; i < arrsz; i++){
    Sa[i] = round(-500/(1+exp(-0.1*((i+1)-80))) + minspeed);
    Sb[i] = round(-500/(1+exp(-0.1*(160-(arrsz-i) - 80))) + (500+maxspeed));
    //Serial.println(Sb[i]);
    }

//  int i = 930;

  Serial.println("Start motor operation");
  int time1 = millis();
  digitalWrite(directionPin, HIGH);
  zoom = minspeed;
  for (int i = 0; i < numberOfSteps; i++){
    //illum = map(zoom, minspeed, maxspeed, 0, 255);
    //analogWrite(ledPin, illum);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(zoom + 4);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(zoom); //Use this to measure runtime of vroomvroom
    zoom = vroomvroom(i);

    }
    int time2 = millis(); //and this
    unsigned int interval = time2 - time1; //etc
    Serial.print("interval = ");
    Serial.println(interval);
  delayMicroseconds(4);

  digitalWrite(directionPin, LOW);
  zoom = minspeed;
  for (int i = 0; i < numberOfSteps; i++){
    //illum = map(zoom, minspeed, maxspeed, 0, 255);
    //analogWrite(ledPin, illum);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(zoom + 4);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(zoom);
    zoom = vroomvroom(i);
  }
  
}

void loop() 
{ 
//digitalWrite(directionPin, HIGH);
//  zoom = minspeed;
//  for (int i = 0; i < numberOfSteps; i++){
//    digitalWrite(stepPin, HIGH);
//    delayMicroseconds(zoom + 4);
//    digitalWrite(stepPin, LOW);
//    delayMicroseconds(zoom);
//    zoom = vroomvroom(i);
//    }
//  delayMicroseconds(4);
//
//  digitalWrite(directionPin, LOW);
//  zoom = minspeed;
//  for (int i = 0; i < numberOfSteps; i++){
//    digitalWrite(stepPin, HIGH);
//    delayMicroseconds(zoom + 4);
//    digitalWrite(stepPin, LOW);
//    delayMicroseconds(zoom);
//    zoom = vroomvroom(i);
//  }
//  delayMicroseconds(4);
}

//Sets speed depending on step interval in which we are
int vroomvroom(int i){
  if (i < t1){    //1st exponential (slow) increase from minspeed
      zoom = Sa[i];
      }
    else if(i >= t1 && i < t2){   //linear increase y=x
      zoom = (minspeed-10) - (i-t1);
      }
    else if(i >= t2 && i < t3){   //2nd exp increase until maxspeed
      zoom = Sb[i-t2];  
      }
    else if(i >= t3 && i < t4){   //continue at maxspeed until motor needs to slow down
      zoom = maxspeed;
      }
    else if(i > t4 && i < t5){    //1st exponential decay from maxspeed
      zoom = Sb[(arrsz-1)-(i-t4)];
      }
    else if(i >= t5 && i < t6){   //linear decrease
      zoom = (maxspeed+10) + (i-t5);  
      }
    else if(i >= t6){   //2nd exponential decay until minspeed
      zoom = Sa[(arrsz-1)-(i-t6)];
      }
      return zoom;
  }
