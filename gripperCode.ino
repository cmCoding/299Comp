#include <Servo.h>
#include <math.h>
#include <QSerial.h>
#define button 2
#define rBump 10
#define lBump 11
QSerial myIRserial;
Servo myservo1, myservo2, myservo3; //1 is pan, 2 is tilt, 3 is gripper
int pan = 100;
int tilt = 160;
float grip = 40;
double distance;
int sensorPin = A0;
float voltage;
int rmd = 7;
int lmd = 4;
int rms = 5;
int lms = 6;

void setup() {
  myIRserial.attach(pin, -1);
  initGripper();
  initBumper();
}
void loop() {

}

void initGripper(){
  myservo1.attach(2);
  myservo2.attach(3);
  myservo3.attach(8);

  myservo1.write(pan);
  myservo2.write(tilt);
  myservo3.write(grip);
}

void initBumper(){
  pinMode(lBump, INPUT);
  pinMode(rBump, INPUT);
  pinMode(rmd, OUTPUT);
  pinMode(lmd, OUTPUT);
  pinMode(rms, OUTPUT);
  pinMode(lms, OUTPUT);
}

void Forward() {
  digitalWrite(rmd, LOW);
  digitalWrite(lmd, LOW);
  analogWrite(rms, 200);
  analogWrite(lms, 200);
}
void pickUpBall() {
  while (pan != 100) {
    myservo1.write(pan);
    pan += 1;
  }
  if (pan == 100) {
    while (tilt != 80) {
      myservo2.write(tilt);
      tilt -= 1;
      delay(10);
    }
  }
  if (pan == 100 && tilt == 80) {
    while (grip <= 130) { //play around with this number so that way it will grab the ball
      myservo3.write(grip);
      grip += 0.5;
      delay(10);
    }
  }

  if (grip > 120) {
    while (tilt != 160) {
      myservo2.write(tilt);
      tilt += 1;
      delay(10);
    }
  }
  return;
}

int wallApproach(int i) {
  if (i == 0)
    return;
  else if (dist() < 20) {
    analogWrite(rms, 100);
    analogWrite(lms, 100);
    if ((digitalRead(rBump)) == LOW && (digitalRead(lBump) == LOW)) {
      analogWrite(rms, 0);
      analogWrite(lms, 0);
      return 1;
    }
    else return 0;
  }
}

void dropBall() {
  while (tilt != 80) {
    myservo2.write(tilt);
    tilt -= 1;
    delay(10);
  }
  if (tilt == 80) {
    while (grip >= 40) {
      myservo3.write(grip);
      grip -= 0.5;
    }
  }
  return;
}

double dist() {
  voltage = map_to_float(analogRead(sensorPin), 0, 1023, 0, 5);
  distance = log10(voltage / 16.083) / -0.817;
  distance = pow(10, distance);
  return distance;
}

void getStart() {
  int check = 0;
  while (check == 0) {
    int val = myIRserial.receive(200);
    if (val == 48) {
      //call left path
      check = 1;
    }
    else if (val == 49) {
      //call middle path
      check = 1;
    }
    else if (val == 50) {
      //call right path
      check = 1;
    }
  }
}

float map_to_float(float x, float a, float b, float c, float d)
{
  float f = x / (b - a) * (d - c) + c;
  return f;
}

void collisionDetect(){
  if (dist() < 20){
    delay(300);
    }
}
