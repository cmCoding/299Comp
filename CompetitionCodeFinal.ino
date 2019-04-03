#include <EEPROM.h>  //needed to access on-chip EEPROM
#include "QSerial.h"
#include <Servo.h>

Servo pan, tilt, clamp;
int pan_angle = 90;
int tilt_angle = 170;
int grip_angle = 40;
byte left_speed, right_speed;  //PWM values from 0 to 255
QSerial myIRboi;

int leftSpeed = 0;
int rightSpeed = 0;
int bcount=0;

#define LSPEED 5
#define LDIR 4
#define RSPEED 6
#define RDIR 7
#define L 13
#define LEFT A2
#define CENTER A1
#define RIGHT A0
#define LTHRESH 950
#define CTHRESH 950
#define RTHRESH 950
#define BUMPL 9
#define BUMPR 8
#define DELAY 400
int encoder_count = 0;
int flag = 0;

void Init() {        //initialize the pin value
  pinMode(RDIR, OUTPUT);
  pinMode(RSPEED, OUTPUT);
  pinMode(LDIR, OUTPUT);
  pinMode(LSPEED, OUTPUT);
  pinMode(L, INPUT);
  pinMode(BUMPL,INPUT);
  pinMode(BUMPR, INPUT);
  pinMode(3, INPUT);
  myIRboi.attach(3, -1);
  int pin0 = analogRead(LEFT);  // left
  int pin1 = analogRead(RIGHT);  // right
  int pin2 = analogRead(CENTER);  // middle
  pan.attach(10);
  tilt.attach(11);
  clamp.attach(12);
  pan.write(pan_angle); // sets to 90°
  tilt.write(tilt_angle);
  clamp.write(grip_angle);
}

int get_position() {
  delay(500);
  int gottem = myIRboi.receive(200);
  if ( gottem > 47 and gottem < 51) {
      Serial.println( char(gottem) );
      return char(gottem);
  }
  return -1;
}

void Forward() {      //driving forward
  digitalWrite(RDIR, HIGH);
  digitalWrite(LDIR, HIGH);
}

void Backup() {       //driving backward
  digitalWrite(RDIR, LOW);
  digitalWrite(LDIR, LOW);
  delay(700);
}


void countEncoder() {     //counting the number of wheel rotation
  if ( digitalRead(L) == 1 and flag == 0 ) {
      encoder_count++;
      flag = 1;
    }
    if ( digitalRead(L) == 0 and flag == 1) {
      encoder_count++;
      flag = 0;
    }
}

void setSpd(int x, int y)  {                 //set the speed of right and left wheel to the input speed
  analogWrite(LSPEED, x);
  analogWrite(RSPEED, y);
}

void setup() {
  Init();
  Serial.begin(9600);
  left_speed = 134;
  right_speed = 117;
  leftSpeed = left_speed;
  rightSpeed = right_speed;
  digitalWrite(RDIR, HIGH);
  digitalWrite(LDIR, HIGH);
  analogWrite(LSPEED, left_speed);
  analogWrite(RSPEED, right_speed);
  }

void turn90right() {        //let the robot turn 90 degree to the right
  digitalWrite(RDIR, HIGH);
  digitalWrite(LDIR, LOW);
  delay(800);
  while (analogRead(CENTER) < CTHRESH) {
  }
  Forward();

}

void turn180() {
  digitalWrite(RDIR, HIGH);
  digitalWrite(LDIR, LOW);
  encoder_count = 0;
  while (encoder_count < 19){
    countEncoder();
  }
  while (analogRead(CENTER) < CTHRESH) {
  }
  Forward();
}

void turn90left() {       // let the robot turn 90 degree to the left
  digitalWrite(RDIR, LOW);
  digitalWrite(LDIR, HIGH);
  delay(800);
  while (analogRead(CENTER) < CTHRESH) {
  }
  Forward();
}


void point1(){
  bcount = 0;
  while(digitalRead(BUMPL) != 0 and digitalRead(BUMPR) != 0){
    Forward();
    driveAlongBlackLine();
      if(bcount == 4){
      delay(DELAY);
      turn90left();
      bcount++;
      }
      if(bcount == 7){
        delay(DELAY);
        turn90right();
        bcount++;
        }
  }
    setSpd(0,0);     //stop the robot 
    
    pickUp();    //activate the gripper (the function is from other groups and will be used in the final competition) 

    setSpd(left_speed,right_speed);
    Backup();              //set to the initial speed 
    turn180();
    bcount = 0;

    while(digitalRead(BUMPL) != 0 and digitalRead(BUMPR) != 0){
      driveAlongBlackLine();
      if(bcount == 1){
      delay(DELAY);
      turn90left();
      bcount++;
      }
      if(bcount == 4){
        delay(DELAY);
        turn90right();
        bcount++;
        }
   }
   setSpd(0,0); 

  yeetIt();    //the function is from other group and will be       used in the final competition 

  //Backup(); 
  turn180();      //turn 180 degrees 
  bcount = 0;  
}

void point2(){
bcount = 0;
while(digitalRead(BUMPL) != 0 and digitalRead(BUMPR) != 0){
Forward();
driveAlongBlackLine();
  if(bcount == 4){
  delay(DELAY);
  turn90right();
  bcount++;
  }
  if(bcount == 7){
    delay(DELAY);
    turn90left();
    bcount++;
    }
}
    setSpd(0,0);     //stop the robot 

    pickUp();       //activate the gripper (the function is from other groups and will be used in the final competition) 

    setSpd(left_speed,right_speed);     //set to the initial speed 
    Backup();
    turn180();
    bcount = 0;

    while(digitalRead(BUMPL) != 0 and digitalRead(BUMPR) != 0){
      driveAlongBlackLine();
      if(bcount == 1){
      delay(DELAY);
      turn90right();
      bcount++;
      }
      if(bcount == 4){
        delay(DELAY);
        turn90left();
        bcount++;
        }
   }
   setSpd(0,0); 

  yeetIt();     //the function is from other group and will be       used in the final competition 

  Backup(); 
  turn180();
  bcount = 0;  
}

void driveAlongBlackLine() {       //keep the robot on the track
  while ( analogRead(CENTER) > CTHRESH and analogRead(RIGHT) < RTHRESH and analogRead(LEFT) < LTHRESH ){
    Forward();
  }
  
 while ( analogRead(CENTER) < CTHRESH and analogRead(RIGHT) > RTHRESH and analogRead(LEFT) < LTHRESH ) {
    adjustRight();      //adjusting the speed when it is not straight
    delay(100);
    Forward();
  }

 while ( analogRead(CENTER) < CTHRESH and analogRead(RIGHT) < RTHRESH and analogRead(LEFT) > LTHRESH ) {
    adjustLeft();                                     //adjusting the speed when it is not straight
    delay(100);
    Forward();
  }
  if ( analogRead(CENTER) > CTHRESH and analogRead(RIGHT) > RTHRESH and analogRead(LEFT) > LTHRESH){
    bcount++;                                         //increment the counter when passing a intersection
    delay(300);
  }
}

void adjustRight() {
  digitalWrite(RDIR,LOW);
  
}

void adjustLeft() {
  digitalWrite(LDIR,LOW);
 
}

void coordinate(int x, int y) {     
//toward the ball
while (digitalRead(BUMPL) != 0 and digitalRead(BUMPR) != 0l){   //use bump to identify whether it’s at wall or not
  Forward();
  driveAlongBlackLine();    //to drive along the black and get the bcount
 if ( bcount == y and x<0){     //to the ball at the left wall
    delay(DELAY);
    turn90left();
    bcount++;              //the purpose of the increment is to let it drive              straight until the bump is pushed
   }
if (bcount == y and x>0){
  delay(DELAY);
  turn90right();        //to the ball at the right wall
bcount++;              //the purpose of the increment is to let it drive straight until the bump is pushed
   }
}
  setSpd(0,0);     //stop the robot
  pickUp();        //activate the gripper (the function is from other groups and will be used in the final competition)
  setSpd(left_speed,right_speed);     //set to the initial speed
  Backup();
  setSpd(0,0);
  delay(20);
  setSpd(134,117);
  turn180();
  bcount = 0;       //set the bcount to 0 and then go back
  bcount = 0;       // return to the basket
while (digitalRead(BUMPL) != 0 and digitalRead(BUMPR) != 0){
  Forward();
  driveAlongBlackLine();
 if ( bcount == abs(x)-1 and x<0){
       delay(DELAY);    
       turn90right();        //turn right and toward the basket
       bcount++;      //the purpose of the increment is to let it drive              straight until the bump is pushed
   }
if (bcount == abs(x)-1 and x>0){
  delay(DELAY);
  turn90left();
  bcount++;   //the purpose of the increment is to let it drive straight until the bump is pushed
   }
}
  //setSpd(0,0);
  yeetIt();     //the function is from other group and will be       used in the final competitio
  Backup();
  setSpd(0,0);
  delay(20);
  setSpd(134,117);
  turn180();
  bcount = 0;
//  setSpd(108,99);      //set to the initial speed
}

/*void approachDie() {
  while (timer < 120) {
    while ((analogRead(RSensor)) < THRESHBLCK  && (analogRead(LSensor)) < THRESHBLCK && (analogRead(CSensor)) > THRESHBLCK) {
      //Drive forward
      digitalWrite(rd, HIGH);
      digitalWrite(ld, HIGH);
      analogWrite(rs, 110);
      analogWrite(ls, 118);
      timer++;
    }
    if ((analogRead(RSensor)) > THRESHBLCK )
    {
      timer++;
      correctRight();
    }
    //if youve gone off the right side of the tape
    if ((analogRead(LSensor)) > THRESHBLCK) {
      timer++;
      correctLeft();
    }
  }
  timer = 0;
}*/

/*void approachBox() {
  while (timer < 420) {
    while ((analogRead(RSensor)) < CTHRESH  && (analogRead(LSensor)) < CTHRESH && (analogRead(CSensor)) > CTHRESH) {
      //Drive forward
      digitalWrite(rd, HIGH);
      digitalWrite(ld, HIGH);
      analogWrite(rs, 110);
      analogWrite(ls, 118);
      timer++;
    }
    if ((analogRead(RSensor)) > THRESHBLCK )
    {
      timer++;
      correctRight();
    }
    //if youve gone off the right side of the tape
    if ((analogRead(LSensor)) > THRESHBLCK) {
      timer++;
      correctLeft();
    }
  }
  timer = 0;
}*/

void pickUp() {

  Serial.println("Picking up");
  digitalWrite(RDIR, LOW);
  digitalWrite(LDIR, LOW);
  analogWrite(RSPEED, right_speed);
  analogWrite(LSPEED, left_speed);
  delay(200);
  analogWrite(RSPEED, 0);
  analogWrite(LSPEED, 0);

  clamp.write(40);
  delay(500);
  tilt.write(75);
  delay(2500);
  clamp.write(165);
  delay(1000);
  tilt.write(170);
  delay(300);
  digitalWrite(RDIR, LOW);
  digitalWrite(LDIR, LOW);
  analogWrite(RSPEED, right_speed);
  analogWrite(LSPEED, left_speed);
}

void yeetIt() {
  Serial.println("Yeeting");
  analogWrite(RSPEED, 0);
  analogWrite(LSPEED, 0);

  tilt.write(90);
  delay(100);
  clamp.write(40);
  delay(600);
  tilt.write(170);
  //new
  delay(300);
  setSpd(left_speed,right_speed);
}

void loop() {                                        // main loop
  Serial.println(get_position());
  if (get_position() == 0){     //do the path 0
//    analogWrite(LSPEED, left_speed);
//    analogWrite(RSPEED, right_speed);
    coordinate(-2,3);
    coordinate(4,5);
    coordinate(0,5);
    coordinate(-2,2);
    coordinate(-2,1);
    setSpd(0,0);
  }
  if (get_position() == 1){      //do the path 1
//    analogWrite(LSPEED, left_speed);
//    analogWrite(RSPEED, right_speed);
    coordinate(-3,4);
    coordinate(3,4);
    coordinate(0,6);
    point1();
    point2();
    setSpd(0,0);
  }
  if (get_position() == 2){      //do the path 2
//    digitalWrite(RDIR, LOW);
//    digitalWrite(LDIR, LOW);
//    analogWrite(LSPEED, left_speed);
//    analogWrite(RSPEED, right_speed);
    coordinate(-4,5);
    coordinate(2,3);
    coordinate(0,6);
    coordinate(2,2);
    coordinate(2,1);
    setSpd(0,0);
  }

//coordinate(1,1);
//Forward();
//
//
//turn90left();
//Forward();
//delay(300);   TEST CODE FOR TWEAKING THE TURNS
//turn180();
//Forward();
//delay(300);

}
