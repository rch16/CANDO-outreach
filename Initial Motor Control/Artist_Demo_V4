// acceleration build-up: 
//    limit jerk implies linear increase of acceleration to the limit acceleration, quadratic increase of speed
// limit acceleration: 
//    implies zero jerk and linear increase of speed
// acceleration ramp-down: 
//    approaching the desired limit velocity with negative limit jerk, i.e. linear decrease of acceleration, (negative) quadratic increase of speed
// limit speed:
//    implies zero jerk and zero acceleration
// deceleration build-up: 
//    limit negative jerk implies linear decrease of acceleration to the negative limit acceleration, (negative) quadratic decrease of speed
// limit deceleration: 
//    implies zero jerk and linear decrease of speed
// deceleration ramp-down: 
//    limit jerk implies linear increase of acceleration to zero, quadratic decrease of speed, approaching the desired position at zero speed and zero acceleration

// connect to the GeckoDrive
const int stepPin = 8;
const int directionPin = 9;

const int totalSteps = 400; // total number of steps (200 in full step)
const float w = 0.005; // pulse width in ms
const float rpm = 100; // speed, revolutions per minute;
const int delaySteps = 50; // number of steps for delay increment/decrement
const float minDelay = 60L * 1000L / delaySteps / rpm; // delay in ms
const float maxDelay = 10*minDelay; // maximum delay in ms;
const float delayValue = (maxDelay - minDelay)/delaySteps; // delay increment/decrement value
float del;

void setup() {
  Serial.begin(9600);
  pinMode(stepPin,OUTPUT); 
  pinMode(directionPin,OUTPUT);
}

void loop() {
    digitalWrite(directionPin,HIGH);
    acceleration_buildUp(10);
    acceleration();
    acceleration_rampDown(10);
    steps(5);
    decelerate();
    digitalWrite(directionPin,LOW);
    acceleration_buildUp(10);
    acceleration();
    acceleration_rampDown(10);
    steps(5);
    decelerate(); 
}

void acceleration_buildUp(int n){
  // linear increase of acceleration 
  // quadratic increase of speed
  del = maxDelay;
  for (int i = 1; i<=n ; i++){
    if (del<=minDelay) break;
    digitalWrite(stepPin, HIGH);
    delay(w);
    digitalWrite(stepPin, LOW);
    delay(del-w);
    del = del - (delayValue*i); // decrement delay between pulses
    }
}

void acceleration(){
  // lineary increase of speed
  for (int i = 1; i<=delaySteps ; i++){
    if (del<=minDelay) break;
    digitalWrite(stepPin, HIGH);
    delay(w);
    digitalWrite(stepPin, LOW);
    delay(del-w);
    del = del - delayValue; // decrement delay between pulses
    }
}

void acceleration_rampDown(int n){
  // linear decrease of acceleration
  // (negative) quadratic increase of speed
  for (int i = 1; i<=n ; i++){
    if (del<=minDelay) break;
    digitalWrite(stepPin, HIGH);
    delay(w);
    digitalWrite(stepPin, LOW);
    delay(del-w);
    del = del - (delayValue/i); // decrement delay between pulses
    }
}

void steps(int n_steps){
  for (int i = 1; i<=n_steps; i++){
    digitalWrite(stepPin, HIGH);
    delay(w);
    digitalWrite(stepPin, LOW);
    delay(minDelay-w);
  }
}

void decelerate(){
  del = minDelay;
  int i = 1, j = 1;
  //for (int i = 1; i<=delaySteps ; i++){
  while(j<delaySteps){
    if (del>=maxDelay) break;
    digitalWrite(stepPin, HIGH);
    delay(w);
    digitalWrite(stepPin, LOW);
    delay(del-w);
    del = del + delayValue; // decrement delay between pulses
  }
}
