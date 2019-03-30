#include <EEPROM.h>  //needed to access on-chip EEPROM
#include "QSerial.h"
#include <Servo.h>

Servo pan, tilt, clamp;
int pan_angle = 90;
int tilt_angle = 170;
int grip_angle = 0;
byte left_speed, right_speed;  //PWM values from 0 to 255
QSerial myIRboi;

int leftSpeed = 0;
int rightSpeed = 0;
int bcount=0;

#define LSPEED 8 // -> 5
#define LDIR 9   //  -> 4
#define RSPEED 10  //-> 7
#define RDIR 11  //  -> 6
#define L 13
#define LEFT A2
#define CENTER A1
#define RIGHT A0
#define LTHRESH 950
#define CTHRESH 950
#define RTHRESH 950
#define BUMP 4
int encoder_count = 0;
int flag = 0;

void Init() {        //initialize the pin value
  pinMode(RDIR, OUTPUT);
  pinMode(RSPEED, OUTPUT);
  pinMode(LDIR, OUTPUT);
  pinMode(LSPEED, OUTPUT);
  pinMode(L, INPUT);
  pinMode(BUMP,INPUT);
  pinMode(3, INPUT);
  myIRboi.attach(3, -1);
  int pin0 = analogRead(LEFT);  // left
  int pin1 = analogRead(RIGHT);  // right
  int pin2 = analogRead(CENTER);  // middle
  pan.attach(5);
  tilt.attach(6);
  clamp.attach(7);
  pan.write(pan_angle); // sets to 90°
  tilt.write(tilt_angle);
  clamp.write(grip_angle);
}

void Forward() {      //driving forward
  digitalWrite(RDIR, HIGH);
  digitalWrite(LDIR, HIGH);
}

void Backup() {       //driving backward
  digitalWrite(RDIR, LOW);
  digitalWrite(LDIR, LOW);
  delay(500);
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
  right_speed = 122;
  leftSpeed = left_speed;
  rightSpeed = right_speed;
  analogWrite(LSPEED, left_speed);
  analogWrite(RSPEED, right_speed);
  }

void turn90right() {        //let the robot turn 90 degree to the right
  encoder_count = 0;
  while (encoder_count < 12) {
  digitalWrite(RDIR, HIGH);
  digitalWrite(LDIR, LOW);
  countEncoder();
  }
}

void turn90left() {       // let the robot turn 90 degree to the left
  encoder_count = 0;
  while (encoder_count < 12) {
  digitalWrite(RDIR, LOW);
  digitalWrite(LDIR, HIGH);
  countEncoder();
  }
}


void point1(){
  while(digitalRead(BUMP) != 0){
  Forward();
  driveAlongBlackLine();
    if(bcount == 1){
    delay(500);
    turn90left();
    bcount++;
    }
    if(bcount == 4){
      delay(500);
      turn90right();
      bcount++;
      }
  }
    setSpd(0,0);     //stop the robot 

    delay(3000); 

//Gripper();        //activate the gripper (the function is from other groups and will be used in the final competition) 

    setSpd(134,122);     //set to the initial speed 
    turn90left(); 
    turn90left();       //To turn 180 degree
    bcount = 0;

    while(digitalRead(BUMP) != 0){
       driveAlongBlackLine();
  if(bcount == 1){
  delay(500);
  turn90left();
  bcount++;
  }
  if(bcount == 4){
    delay(500);
    turn90right();
    bcount++;
    }
   }
   setSpd(0,0); 

//Dropball();     //the function is from other group and will be       used in the final competition 

  Backup(); 
  turn90left(); 
  turn90left();       //turn 180 degrees 
  bcount = 0;  
}

void point2(){
while(digitalRead(BUMP) != 0){
Forward();
driveAlongBlackLine();
  if(bcount == 1){
  delay(500);
  turn90right();
  bcount++;
  }
  if(bcount == 4){
    delay(500);
    turn90left();
    bcount++;
    }
}
    setSpd(0,0);     //stop the robot 

    delay(3000); 

//Gripper();        //activate the gripper (the function is from other groups and will be used in the final competition) 

    setSpd(134,122);     //set to the initial speed 
    turn90left(); 
    turn90left();       //To turn 180 degree
    bcount = 0;

    while(digitalRead(BUMP) != 0){
       driveAlongBlackLine();
  if(bcount == 1){
  delay(500);
  turn90right();
  bcount++;
  }
  if(bcount == 4){
    delay(500);
    turn90left();
    bcount++;
    }
   }
   setSpd(0,0); 

//Dropball();     //the function is from other group and will be       used in the final competition 

  Backup(); 
  turn90left(); 
  turn90left();       //turn 180 degrees 
  bcount = 0;  
}

void driveAlongBlackLine() {       //keep the robot on the track
  while ( analogRead(CENTER) > CTHRESH and analogRead(RIGHT) < RTHRESH and analogRead(LEFT) < LTHRESH ){
    Forward();
  }
  
 while ( analogRead(CENTER) < CTHRESH and analogRead(RIGHT) > RTHRESH and analogRead(LEFT) < LTHRESH ) {
    adjustRight();      //adjusting the speed when it is not straight
    delay(50);
    Forward();
  }

  // veers right
 while ( analogRead(CENTER) < CTHRESH and analogRead(RIGHT) < RTHRESH and analogRead(LEFT) > LTHRESH ) {
    adjustLeft();                                     //adjusting the speed when it is not straight
    delay(50);
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
while (digitalRead(BUMP)!= 0){   //use bump to identify whether it’s at wall or not
  Forward();
  driveAlongBlackLine();    //to drive along the black and get the bcount
 if ( bcount == y and x<0){     //to the ball at the left wall
    delay(500);
    turn90left();
    bcount++;              //the purpose of the increment is to let it drive              straight until the bump is pushed
   }
if (bcount == y and x>0){
  delay(500);
  turn90right();        //to the ball at the right wall
bcount++;              //the purpose of the increment is to let it drive straight until the bump is pushed
   }
}
  setSpd(0,0);     //stop the robot
  delay(3000);
  //Gripper();        //activate the gripper (the function is from other groups and will be used in the final competition)
  setSpd(134,122);     //set to the initial speed
  turn90left();
  turn90left();       //To turn 180 degree
  bcount = 0;       //set the bcount to 0 and then go back
  bcount = 0;       // return to the basket
while (digitalRead(BUMP) != 0){
  Forward();
  driveAlongBlackLine();
 if ( bcount == abs(x) and x<0){
       delay(500);    
       turn90right();        //turn right and toward the basket
       bcount++;      //the purpose of the increment is to let it drive              straight until the bump is pushed
   }
if (bcount == abs(x) and x>0){
  delay(500);
  turn90left();
  bcount++;   //the purpose of the increment is to let it drive straight until the bump is pushed
   }
}
  //setSpd(0,0);
  //Dropball();     //the function is from other group and will be       used in the final competition
  Backup();
  turn90left();
  turn90left();       //turn 180 degrees
  bcount = 0;
//  setSpd(108,99);      //set to the initial speed
}

int get_position() {
  int gottem = myIRboi.receive(200);
  if ( gottem > 46 and gottem < 58) {
      Serial.println( char(gottem) );
      return char(gottem);
  }
  delay(100);
}
 
void loop() {                                        // main loop
//  if (get_position() == 0){     //do the path 0
    coordinate(-2,3);
    coordinate(4,5);
    coordinate(0,6);
    coordinate(-2,2);
    coordinate(-2,1);
    setSpd(0,0);
//  }
  if (get_position() == 1){      //do the path 1
    coordinate(-3,4);
    coordinate(3,4);
    coordinate(0,6);
    point1();
    point2();
  }
  if (get_position() == 2){      //do the path 2
    coordinate(-4,5);
    coordinate(2,3);
    coordinate(0,6);
    coordinate(2,2);
    coordinate(2,1);
  }
}
