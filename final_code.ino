

#include <Enes100.h>

//right motors
//front wheel
int PIN_MOTOR_1_FORWARD = 3;
int PIN_MOTOR_1_REVERSE = 2;

//back wheel
int PIN_MOTOR_2_FORWARD = 4;
int PIN_MOTOR_2_REVERSE = 5;

//left motors
//front wheel
int PIN_MOTOR_3_FORWARD = 11;
int PIN_MOTOR_3_REVERSE = 10;

//back wheel
int PIN_MOTOR_4_FORWARD = 8;
int PIN_MOTOR_4_REVERSE = 9;

const int waterPumpBlack = 22;
const int waterPumpRed = 23;

const int pingPin = 42;  // Trigger Pin of Ultrasonic Sensor
const int echoPin = 43;  // Echo Pin of Ultrasonic Sensor

//Downward facing ultrasonic sensor
const int downPingPin = 46;  // Trigger Pin of Ultrasonic Sensor
const int downEchoPin = 47;  // Echo Pin of Ultrasonic Sensor

//Color sensor
const int rgbSCL = A0;
const int rgbSDA = A1;

//Conductivity sensor
const int conductivityPin = A8;

#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0, temperature = 25;

boolean inFrontOfObstacle = false;
boolean salt = false;


int control = 0;

void setup() {
  delay(3000);
  Enes100.begin("Elephant", WATER, 207, 50, 51);
  delay(1000);
  Enes100.updateLocation();
  Enes100.println("Hey we started");
  delay(500);
  Enes100.println("In setup; about to print y location");
  Enes100.println(Enes100.location.y);
  Enes100.println("In setup; about to print x location");
  Enes100.println(Enes100.location.x);
  Enes100.println("Printing mission site y: ");
  Enes100.println(Enes100.missionSite.y);


  //Right side
  pinMode(PIN_MOTOR_1_FORWARD, OUTPUT);  //motor connections
  pinMode(PIN_MOTOR_1_REVERSE, OUTPUT);
  pinMode(PIN_MOTOR_2_FORWARD, OUTPUT);
  pinMode(PIN_MOTOR_2_REVERSE, OUTPUT);

  //Left side
  pinMode(PIN_MOTOR_3_FORWARD, OUTPUT);  //motor connections
  pinMode(PIN_MOTOR_3_REVERSE, OUTPUT);
  pinMode(PIN_MOTOR_4_FORWARD, OUTPUT);
  pinMode(PIN_MOTOR_4_REVERSE, OUTPUT);

  pinMode(waterPumpBlack, OUTPUT);
  pinMode(waterPumpRed, OUTPUT);
  
  pinMode(conductivityPin, INPUT);
}

void loop() {
  Enes100.updateLocation();
  delay(750);
  if (control == 0) {
    missionSite();
    delay(5000);
    if (2 - Enes100.missionSite.y < 1.0) {
      while (abs(Enes100.location.y - 1.5) > 0.1) {
        moveBackward();
        delay(500);
        stop();
        delay(750);
        Enes100.updateLocation();
        delay(750);
      }
      turnLeftAngle(0.0);
    } else {
      while (abs(Enes100.location.y - 0.5) > 0.1) {
        moveBackward();
        delay(500);
        stop();
        delay(750);
        Enes100.updateLocation();
        delay(750);
      }
      turnRightAngle(0.0);
    }
    control = 1;
  }  
  if(control == 1)
  {
    navigate();
  }
}  
  
void navigate() {
  moveToObstacle();
  delay(750);
  if (inFrontOfObstacle == true) {
    Enes100.updateLocation();
    delay(500);
    Enes100.println(Enes100.location.y);
    if (Enes100.location.y > 1.0) {
      while (getDistance() < 150) {
        goSidewaysRight();
        delay(250);
        stop();
        delay(250);
      }
      Enes100.updateLocation();
      delay(500);
      float target = Enes100.location.y - .3;
      while (abs(target - Enes100.location.y) >= 0.05) {
        goSidewaysRight();
        delay(100);
        stop();
        delay(250);
        Enes100.updateLocation();
        delay(500);
      }
      inFrontOfObstacle = false;
    } else {
      while (getDistance() < 150) {
        goSidewaysLeft();
        delay(250);
        stop();
        delay(250);
      }
      Enes100.updateLocation();
      delay(500);
      float target = Enes100.location.y + .3;
      while (target - Enes100.location.y >= 0.05) {
        goSidewaysLeft();
        delay(100);
        stop();
        delay(250);
        Enes100.updateLocation();
        delay(500);
      }
      inFrontOfObstacle = false;
    }
  }
}
void moveToObstacle() {
   if (Enes100.location.theta < -0.0872665 || Enes100.location.theta > 0.0872665) {
    if (Enes100.location.theta < 0) {
      turnLeftAngle(0.0);
    } else {
      turnRightAngle(0.0);
    }
  }
  while (getDistance() > 150 && Enes100.location.x < 3.0) {
    moveForward();
    delay(500);
    stop();
    delay(250);
    Enes100.updateLocation();
    delay(500);
  }
  Enes100.updateLocation();
  delay(250);
  if (Enes100.location.x < 2.5) {
    inFrontOfObstacle = true;
    Enes100.updateLocation();
    delay(500);
  } else {
    limbo();
  }
}

void missionSite() {
  Enes100.updateLocation();
  delay(500);
  if (Enes100.location.y > 1.0) {
    turnRightAngle(-1.57079632679);
  } else {
    turnRightAngle(1.57079632679);
  }
  float site = 2 - Enes100.missionSite.y;
  float diffY = site - Enes100.location.y;
  while (abs(diffY) >= 0.05) {
    moveForward();
    delay(150);
    stop();
    delay(250);
    Enes100.updateLocation();
    delay(500);
    diffY = site - Enes100.location.y;
  }
  stop();
  Enes100.updateLocation();
  delay(250);
  //depth
  int d = waterDepth();
  Enes100.print("The water depth is: ");
  Enes100.println(d);
  Enes100.mission(DEPTH, d);
  //pump
  drawWater();
  delay(1000);
  //color
}

void limbo() {
  Enes100.updateLocation();
  delay(500);
  while (abs(Enes100.location.y - 1.5) > .1) {
    if (Enes100.location.y > 1.5) {
      goSidewaysRight();
      delay(250);
      stop();
      delay(250);
    } else {
      goSidewaysLeft();
      delay(250);
      stop();
      delay(250);
    }
    Enes100.updateLocation();
    delay(500);
  }
  while (Enes100.location.x < 3.85) {
    moveForward();
    delay(500);
    stop();
    delay(250);
    Enes100.updateLocation();
    delay(500);
  }
  stop();
  control = 2;
}

void pump() {
  digitalWrite(waterPumpRed, HIGH);
  digitalWrite(waterPumpBlack, LOW);
}

void stopPump() {
  digitalWrite(waterPumpRed, LOW);
  digitalWrite(waterPumpBlack, LOW);
}

void drawWater() {
  pump();
  delay(30000);
  stopPump();
}

long waterDepth() {
  return 90 - getDistanceDown() - 10;
}

void moveForward() {
  digitalWrite(PIN_MOTOR_1_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_1_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_2_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_2_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_3_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_3_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_4_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_4_REVERSE, LOW);
}

void moveBackward() {
  digitalWrite(PIN_MOTOR_1_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_1_REVERSE, HIGH);
  digitalWrite(PIN_MOTOR_2_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_2_REVERSE, HIGH);
  digitalWrite(PIN_MOTOR_3_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_3_REVERSE, HIGH);
  digitalWrite(PIN_MOTOR_4_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_4_REVERSE, HIGH);
}



void turnRight() {
  digitalWrite(PIN_MOTOR_1_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_1_REVERSE, HIGH);
  digitalWrite(PIN_MOTOR_2_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_2_REVERSE, HIGH);
  digitalWrite(PIN_MOTOR_3_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_3_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_4_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_4_REVERSE, LOW);
}


void turnLeft() {
  digitalWrite(PIN_MOTOR_1_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_1_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_2_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_2_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_3_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_3_REVERSE, HIGH);
  digitalWrite(PIN_MOTOR_4_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_4_REVERSE, HIGH);
}

void stop() {
  digitalWrite(PIN_MOTOR_1_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_1_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_2_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_2_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_3_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_3_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_4_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_4_REVERSE, LOW);
}


void goSidewaysRight() {
  digitalWrite(PIN_MOTOR_1_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_1_REVERSE, HIGH);
  digitalWrite(PIN_MOTOR_2_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_2_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_3_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_3_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_4_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_4_REVERSE, HIGH);
}

void goSidewaysLeft() {
  digitalWrite(PIN_MOTOR_1_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_1_REVERSE, LOW);
  digitalWrite(PIN_MOTOR_2_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_2_REVERSE, HIGH);
  digitalWrite(PIN_MOTOR_3_FORWARD, LOW);
  digitalWrite(PIN_MOTOR_3_REVERSE, HIGH);
  digitalWrite(PIN_MOTOR_4_FORWARD, HIGH);
  digitalWrite(PIN_MOTOR_4_REVERSE, LOW);
}



void turnRightAngle(float theta) {
  Enes100.updateLocation();
  delay(500);
  float tDiff = theta - Enes100.location.theta;
  while (abs(tDiff) >= 0.0872665) {
    turnRight();
    delay(50);
    stop();
    delay(50);
    Enes100.updateLocation();
    delay(500);
    tDiff = theta - Enes100.location.theta;
  }
  stop();
}

void turnLeftAngle(float theta) {
  Enes100.updateLocation();
  delay(500);
  float tDiff = theta - Enes100.location.theta;
  while (abs(tDiff) >= 0.0872665) {
    turnLeft();
    delay(50);
    stop();
    delay(50);
    Enes100.updateLocation();
    delay(500);
    tDiff = theta - Enes100.location.theta;
  }
  stop();
}




long getDistance() {
  long duration, inches, cm, mm;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  mm = centimetersToMillimeters(cm);
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(100);
  return mm;
}

long getDistanceDown() {
  long duration, inches, cm, mm;
  pinMode(downPingPin, OUTPUT);
  digitalWrite(downPingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(downPingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(downPingPin, LOW);
  pinMode(downEchoPin, INPUT);
  duration = pulseIn(downEchoPin, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  mm = centimetersToMillimeters(cm);
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(100);
  return mm;
}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

long centimetersToMillimeters(long centimeters) {
  return centimeters * 10;
}
