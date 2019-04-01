#define THRESHBLCK 890
//#define THRESHOFF 950
//Receiver

//motors
int rs = 6;
int rd = 7; //direction
int ls = 5;
int ld = 4; //direction
//line sensors
int LSensor = A1;
int CSensor = A3 ;
int RSensor = A2 ;
//crosroads
int crossCount = 0;
//encoder
int leftEncoder = 13;
int rightEncoder = 12;
int counter = 0;
//bumpers
int leftbumper = 10;
int rightbumper = 9;
//Servos
#include <Servo.h>
Servo pan, tilt, grip;
//aproaching timer
int timer = 0;
//turn around
int linesPassed = 0;
//beacons
char leftPath = '0';
char centerPath = '1';
char rightPath = '2';
boolean onLeft = false;
boolean onCenter = true;
boolean onRight = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //directional motors
  pinMode(rd, OUTPUT);
  pinMode(ld, OUTPUT);
  //encoders
  pinMode(leftEncoder, INPUT);
  pinMode(rightEncoder, INPUT);
  //bumpers
  pinMode(leftbumper, INPUT);
  pinMode(rightbumper, INPUT);
  //servos
  pan.attach(2);
  grip.attach(3);
  tilt.attach(8);
  //starting servo positions
  Serial.println("Initializing servos");
  tilt.write(170);
  grip.write(40); //open
  pan.write(100); //centered
  Serial.println("Done initializing servos");
  delay(200);


}
void loop() {
  //While on the line
  //reactBumper();
  if (digitalRead(leftbumper) == true) {
    onLeft = true;
    crossCount = 0;
    forward();
  }
  if (onCenter == true) {
    onCenter = true;
    crossCount = 0;
    forward();
  }
  if (digitalRead(rightbumper) == true) {
    onRight = true;
    crossCount = 0;
    forward();
  }

}

void forward() {
  while (true) {
    while ((analogRead(RSensor)) < THRESHBLCK  && (analogRead(LSensor)) < THRESHBLCK && (analogRead(CSensor)) > THRESHBLCK) {
      Serial.println("1st while loop");
      //Drive forward
      digitalWrite(rd, HIGH);
      digitalWrite(ld, HIGH);
      analogWrite(rs, 110);
      analogWrite(ls, 118);
    }

    //When detecting an intersection
    if (((analogRead(RSensor)) > THRESHBLCK  && (analogRead(LSensor)) > THRESHBLCK && (analogRead(CSensor)) > THRESHBLCK)) {
      Serial.println("if crossroad");
      //Keep track of intersections passed
      crossCount++;
      Serial.println(crossCount);
      //Drive forward to potential turning position
      digitalWrite(rd, HIGH);
      digitalWrite(ld, HIGH);
      analogWrite(rs, 110);
      analogWrite(ls, 118);
      delay(100);
      //check if you need to turn
      checkCrossroads();
    }

    //If youve gone off the left side of the tape
    if ((analogRead(RSensor)) > THRESHBLCK) {
      correctRight();
    }
    //if youve gone off the right side of the tape
    if ((analogRead(LSensor)) > THRESHBLCK) {
      correctLeft();
    }
  }
}

void approach() {
  while (true) {
    while ((analogRead(RSensor)) < THRESHBLCK  && (analogRead(LSensor)) < THRESHBLCK && (analogRead(CSensor)) > THRESHBLCK) {
      //Drive forward
      digitalWrite(rd, HIGH);
      digitalWrite(ld, HIGH);
      analogWrite(rs, 110);
      analogWrite(ls, 118);
      if (digitalRead(leftbumper) == HIGH || digitalRead(rightbumper) == HIGH) {
        reactBumper();
      }
    }
    //When detecting an intersection

    //If youve gone off the left side of the tape
    if ((analogRead(RSensor)) > THRESHBLCK )
    {
      if (digitalRead(leftbumper) == HIGH || digitalRead(rightbumper) == HIGH) {
        reactBumper();
      }
      correctRight();


    }
    //if youve gone off the right side of the tape
    if ((analogRead(LSensor)) > THRESHBLCK) {
      if (digitalRead(leftbumper) == HIGH || digitalRead(rightbumper) == HIGH) {
        reactBumper();
      }
      correctLeft();

    }
     //reactBumper();
  }
}


void turnLeft() {
  //turn left until detecting the tape
  Serial.println("turn left");
  digitalWrite(rd, HIGH);
  digitalWrite(ld, LOW);
  analogWrite(rs, 100);
  analogWrite(ls, 108);
  delay(500);
  while (analogRead(CSensor) < THRESHBLCK) {
    //Serial.println(CSensor);
  }
  forward();
}

void turnLeft2() {
  //turn left until detecting the tape
  Serial.println("turn left");
  digitalWrite(rd, HIGH);
  digitalWrite(ld, LOW);
  analogWrite(rs, 100);
  analogWrite(ls, 108);
  delay(500);
  while (analogRead(CSensor) < THRESHBLCK) {
    //Serial.println(CSensor);
  }
  approach();
}

void turnRight() {
  //turn right until detecting tape, delay to ensure it doesnt pick up current line
  Serial.println("turn right");
  digitalWrite(rd, LOW);
  digitalWrite(ld, HIGH);
  analogWrite(rs, 100);
  analogWrite(ls, 108);
  delay(800);
  while (analogRead(CSensor) < THRESHBLCK) {
    // Serial.println(analogRead(CSensor));
  }
  forward();
}

void turnRight2() {
  //turn right until detecting tape, delay to ensure it doesnt pick up current line
  Serial.println("turn right");
  digitalWrite(rd, LOW);
  digitalWrite(ld, HIGH);
  analogWrite(rs, 100);
  analogWrite(ls, 108);
  delay(800);
  while (analogRead(CSensor) < THRESHBLCK) {
    // Serial.println(analogRead(CSensor));
  }
  approach();
}

void turnAround() {
  //turn right until detecting tape, delay to ensure it doesnt pick up current line

  Serial.println("turn around");
  digitalWrite(rd, LOW);
  digitalWrite(ld, LOW);
  analogWrite(rs, 110);
  analogWrite(ls, 118);
  delay(600);
  digitalWrite(ld, HIGH);
  digitalWrite(rd, LOW);
  analogWrite(rs, 110);
  analogWrite(ls, 118);
  linesPassed = 0;
  delay(600);
  while (linesPassed < 1) {
    if (analogRead(CSensor) > THRESHBLCK) {
      linesPassed++;
      delay(100);
    }
  }
  delay(100);
  while (analogRead(CSensor) < THRESHBLCK) {

    Serial.println(analogRead(CSensor));
  }
  Serial.println("stop");
  //delay(50);
  forward();

}

void turnAroundStart() {
  //turn right until detecting tape, delay to ensure it doesnt pick up current line
  Serial.println("turn around");
  digitalWrite(rd, LOW);
  digitalWrite(ld, LOW);
  analogWrite(rs, 110);
  analogWrite(ls, 118);
  delay(400);
  digitalWrite(ld, HIGH);
  digitalWrite(rd, LOW);
  analogWrite(rs, 110);
  analogWrite(ls, 118);
  delay(400);
  while (analogRead(CSensor) < THRESHBLCK) {

    Serial.println(analogRead(CSensor));
  }
  Serial.println("stop");
  //delay(50);
  forward();

}

void correctLeft() {
  //correct left until on the tape again
  Serial.println("correct left");
  digitalWrite(rd, HIGH);
  digitalWrite(ld, LOW);
  analogWrite(rs, 120);
  analogWrite(ls, 95);

  while (LSensor > THRESHBLCK) {
    //Serial.println(LSensor);
  }
}

void correctRight() {
  //correct right until on the tape again
  Serial.println("correct right");
  digitalWrite(rd, HIGH);
  digitalWrite(ld, HIGH);
  analogWrite(rs, 95);
  analogWrite(ls, 128);

  while (RSensor > THRESHBLCK) {
    //Serial.println(RSensor);
  }
}

void checkCrossroads() {
  //set for the left path
  //predetermined actions for a set path, determined by how many crossroads have been passed
  //turn left
  if (onLeft == true) {
    if (crossCount == 3 || crossCount == 18 || crossCount == 32 || crossCount == 36) {
      countEncoder();
      turnLeft();
    }
    //turn right
    else if (crossCount == 5 || crossCount == 12 || crossCount == 38) {
      countEncoder();
      turnRight();
    }
    //turn right and pick up | special case
    else if (crossCount == 34) {
      countEncoder();
      turnRight2();
    }
    //pick up
    else if (crossCount == 4 || crossCount == 15 || crossCount == 27 || crossCount == 33 || crossCount == 37) {
      approach();
    }
    //drop off
    else if (crossCount == 7 || crossCount == 22 || crossCount == 31 || crossCount == 39) {
      approach();
    }
  }

  else if (onCenter == true) {
    if (crossCount == 4 || crossCount == 19 || crossCount == 36 || crossCount == 39 || crossCount == 54) {
      countEncoder();
      turnLeft();
    }
    //turn right
    else if (crossCount == 8 || crossCount == 15 || crossCount == 41 || crossCount == 49 || crossCount == 52) {
      countEncoder();
      turnRight();
    }
    //turn right and pick up | special case
    else if (crossCount == 38) {
      countEncoder();
      turnRight2();
    }
    //turn left and pick up special case
    else if (crossCount == 51) {
      countEncoder();
      turnLeft2();
    }
    //pick up
    else if (crossCount == 6 || crossCount == 17 || crossCount == 27) {
      approach();
    }
    //drop off
    else if (crossCount == 11 || crossCount == 22 || crossCount == 31 || crossCount == 44 || crossCount == 57) {
      approach();
    }
  }

  else if (onRight == true) {
    if (crossCount == 5 || crossCount == 20 || crossCount == 38) {
      countEncoder();
      turnLeft();
    }
    //turn right
    else if (crossCount == 11 || crossCount == 18 || crossCount == 32 || crossCount == 36) {
      countEncoder();
      turnRight();
    }
    //turn left and pick up | special case
    else if (crossCount == 34) {
      countEncoder();
      turnLeft2();
    }
    //pick up
    else if (crossCount == 8 || crossCount == 19 || crossCount == 27 || crossCount == 33 || crossCount == 37) {
      approach();
    }
    //drop off
    else if (crossCount == 15 || crossCount == 22 || crossCount == 31 || crossCount == 39) {
      approach();
    }
  }
}

void countEncoder() {
  //amount of encoders to reach valid turning position
  Serial.println("count encoders");
  counter = 0;
  while (counter < 5) {
    while (digitalRead(leftEncoder) == HIGH) {

    }
    counter++;

    Serial.println(counter);

    while (digitalRead(leftEncoder) == LOW) {

    }
    counter++;
    Serial.println(counter);
  }
}



void reactBumper() {

  while (digitalRead(rightbumper) == LOW || digitalRead(leftbumper) == LOW) {
    if (digitalRead(rightbumper) == HIGH) {
      Serial.println("Right bumper triggered");
      digitalWrite(rd, LOW);
      digitalWrite(ld, HIGH);
      analogWrite(rs, 0);
      analogWrite(ls, 200);
    }
    else if (digitalRead(leftbumper) == HIGH) {
      Serial.println("Left bumper triggered");
      digitalWrite(rd, HIGH);
      digitalWrite(ld, LOW);
      analogWrite(rs, 200);
      analogWrite(ls, 0);

    }

  }
  Serial.println("Both bumpers triggered");
  //turnAround();
  if (onLeft == true) {
    if (crossCount == 4 || crossCount == 15 || crossCount == 27 || crossCount == 33 || crossCount == 37) {
      pickUp();
    }
    if (crossCount == 7 || crossCount == 22 || crossCount == 31 || crossCount == 34 ) {
      yeetIt();
    }
    if (crossCount == 39) {
      yeetIt();
    }
  }
  else if (onCenter == true) {
    if (crossCount == 6 || crossCount == 17 || crossCount == 27 || crossCount == 38 || crossCount == 51) {
      pickUp();
    }
    if (crossCount == 11 || crossCount == 22 || crossCount == 31 || crossCount == 44 ) {
      yeetIt();
    }
    if (crossCount == 57) {
      yeetIt();
    }

  }
  else if (onRight == true) {
    if (crossCount == 8 || crossCount == 19 || crossCount == 27 || crossCount == 33 || crossCount == 37) {
      pickUp();
    }
    if (crossCount == 15 || crossCount == 22 || crossCount == 31 || crossCount == 34 ) {
      yeetIt();
    }
    if (crossCount == 39) {
      yeetIt();
    }
  }
}

void pickUp() {

  Serial.println("Picking up");
  digitalWrite(rd, LOW);
  digitalWrite(ld, LOW);
  analogWrite(rs, 110);
  analogWrite(ls, 118);
  delay(400);
  analogWrite(rs, 0);
  analogWrite(ls, 0);
  delay(200);
  for (int i = 170; i > 75; i--) {
    tilt.write(i);
    delay(50);
  }

  delay(800);
  grip.write(143);
  delay(600);
  tilt.write(170);
  delay(300);
  digitalWrite(rd, LOW);
  digitalWrite(ld, LOW);
  analogWrite(rs, 110);
  analogWrite(ls, 118);
  delay(400);
  turnAround();
}

void yeetIt() {
  Serial.println("Yeeting");
  analogWrite(rs, 0);
  analogWrite(ls, 0);

  tilt.write(90);
  delay(40);
  grip.write(40);
  delay(600);
  tilt.write(170);
  //new
  delay(300);
  if (onLeft == true) {
    if (crossCount == 39) {
      analogWrite(ls, 0);
      analogWrite(rs, 0);
      onLeft = false;
      //onCenter = true;
      while (1) {
        
      }
    }
    
  }
  else if (onCenter == true) {
    if (crossCount == 57) {
      analogWrite(ls, 0);
      analogWrite(rs, 0);
      onCenter = false;
      //onRight = true;
      while (1) {
        
      }
    }
  }
  else if (onRight == true) {
    if (crossCount == 39) {
      analogWrite(ls, 0);
      analogWrite(rs, 0);
      onRight = false;
      //onLeft = true;
      delay(200);
      while (1) {
        
      }
    }
  }
  turnAroundStart();
}
