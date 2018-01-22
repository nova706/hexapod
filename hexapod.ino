#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <HexJoint.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define FREQUENCY 50

#define SERVOMIN    650 // 860 // spec: 500
#define SERVOMAX    2350 // 2140 // spec: 2400

#define HORIZ_MAX   120
#define HORIZ_DEF   90
#define HORIZ_MIN   60
#define VERT_MAX    120
#define VERT_DEF    80
#define VERT_MIN    60

HexJoint flHip = HexJoint();
HexJoint mlHip = HexJoint();
HexJoint blHip = HexJoint();
HexJoint frHip = HexJoint();
HexJoint mrHip = HexJoint();
HexJoint brHip = HexJoint();
HexJoint flKnee = HexJoint();
HexJoint mlKnee = HexJoint();
HexJoint blKnee = HexJoint();
HexJoint frKnee = HexJoint();
HexJoint mrKnee = HexJoint();
HexJoint brKnee = HexJoint();

int serialData = 0;// data and serial data are just variables to hold information.
int data = 0;

bool standing = false;

void setup() 
{
  Serial.begin(9600);
  Serial.println("16 channel Servo test!");
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);

  flHip.begin(2, false, true);
  mlHip.begin(4, false, true);
  blHip.begin(6, false, true);
  frHip.begin(10, false, false);
  mrHip.begin(12, false, false);
  brHip.begin(14, false, false);

  flKnee.begin(3, true, true);
  mlKnee.begin(5, true, true);
  blKnee.begin(7, true, true);
  frKnee.begin(11, true, false);
  mrKnee.begin(13, true, false);
  brKnee.begin(15, true, false);

  stand();
}

void loop() {
  if (Serial.available() > 0) { //Serial.available will tell the board to look at the information being sent to it through the serial port.
 
    serialData = Serial.read(); //The arduino reads this data and writes it as one of our variables.
    Serial.println(serialData); //The arduino prints this data to the serial monitor so we can see what it sees.

    data = serialData;
  }
  if (data == 53) {
    if (standing) {
      sit();
    } else {
      stand();
    }
    data = 0;
  } else if (data == 56) {
    walk(1);
  } else if (data == 50) {
    walk(-1);
  } else if (data == 57) {
    turn(1);
  } else if (data == 55) {
    turn(-1);
  } else if (data == 54) {
    strafe(1);
  } else if (data == 52) {
    strafe(-1);
  }
}

int pulseWidth(int angle) {
  Serial.println(angle);

  int pulse_wide, analog_value;
  pulse_wide = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  analog_value = (float(pulse_wide) * FREQUENCY * 4096) / 1000000;
  return analog_value;
}

void stand() {
  Serial.println("stand");

  setPos(flHip, 0);
  setPos(mlHip, 0);
  setPos(blHip, 0);
  setPos(frHip, 0);
  setPos(mrHip, 0);
  setPos(brHip, 0);

  delay(500);

  setPos(flKnee, 0);
  setPos(mlKnee, 0);
  setPos(blKnee, 0);
  setPos(frKnee, 0);
  setPos(mrKnee, 0);
  setPos(brKnee, 0);
  
  standing = true;
  delay(1000);
  stopMoving();
}

void sit() {
  Serial.println("sit");
  
  setPos(flKnee, 2);
  setPos(mlKnee, 2);
  setPos(blKnee, 2);
  setPos(frKnee, 2);
  setPos(mrKnee, 2);
  setPos(brKnee, 2);

  delay(500);

  setPos(flHip, 0);
  setPos(mlHip, 0);
  setPos(blHip, 0);
  setPos(frHip, 0);
  setPos(mrHip, 0);
  setPos(brHip, 0);
  
  standing = false;
  delay(1000);
  stopMoving();
}

void walk(int dir) {
  Serial.println("walk");

  int dirPos = 1;
  if (dir == -1) {
    dirPos = 2;
  }

  setPos(flKnee, 1);
  setPos(blKnee, 1);
  setPos(mrKnee, 1);

  delay(500);

  setPos(flHip, dirPos);
  setPos(blHip, dirPos);
  setPos(mrHip, dirPos);

  delay(500);

  setPos(flKnee, 0);
  setPos(blKnee, 0);
  setPos(mrKnee, 0);

  delay(500);
  
  setPos(mlKnee, 1);
  setPos(frKnee, 1);
  setPos(brKnee, 1);

  delay(500);

  setPos(mlHip, dirPos);
  setPos(frHip, dirPos);
  setPos(brHip, dirPos);

  delay(500);
  
  setPos(mlKnee, 0);
  setPos(frKnee, 0);
  setPos(brKnee, 0);

  delay(500);

  setPos(flHip, 0);
  setPos(mlHip, 0);
  setPos(blHip, 0);
  setPos(frHip, 0);
  setPos(mrHip, 0);
  setPos(brHip, 0);
  
  standing = false;
  delay(500);
}

void turn(int dir) {
  Serial.println("turn");

  int forwardPos = 1;
  int backwardPos = 2;
  if (dir == -1) {
    forwardPos = 2;
    backwardPos = 1;
  }

  setPos(flKnee, 1);
  setPos(blKnee, 1);
  setPos(mrKnee, 1);

  delay(500);

  setPos(flHip, forwardPos);
  setPos(blHip, forwardPos);
  setPos(mrHip, backwardPos);

  delay(500);

  setPos(flKnee, 0);
  setPos(blKnee, 0);
  setPos(mrKnee, 0);

  delay(500);
  
  setPos(mlKnee, 1);
  setPos(frKnee, 1);
  setPos(brKnee, 1);

  delay(500);

  setPos(mlHip, forwardPos);
  setPos(frHip, backwardPos);
  setPos(brHip, backwardPos);

  delay(500);
  
  setPos(mlKnee, 0);
  setPos(frKnee, 0);
  setPos(brKnee, 0);

  delay(500);

  setPos(flHip, 0);
  setPos(mlHip, 0);
  setPos(blHip, 0);
  setPos(frHip, 0);
  setPos(mrHip, 0);
  setPos(brHip, 0);
  
  standing = false;
  delay(500);
}

void strafe(int dir) {
  Serial.println("strafe");

  int forwardPos = 1;
  int backwardPos = 2;
  if (dir == -1) {
    forwardPos = 2;
    backwardPos = 1;
  }

  setPos(flKnee, 1);
  setPos(brKnee, 1);

  delay(500);

  setPos(flHip, forwardPos);
  setPos(brHip, forwardPos);

  delay(500);

  setPos(flKnee, 0);
  setPos(brKnee, 0);

  delay(500);
  
  setPos(frKnee, 1);
  setPos(blKnee, 1);

  delay(500);

  setPos(frHip, backwardPos);
  setPos(blHip, backwardPos);

  delay(500);
  
  setPos(frKnee, 0);
  setPos(blKnee, 0);

  delay(500);
  
  setPos(mrKnee, 1);
  setPos(mlKnee, 1);

  delay(500);

  setPos(flHip, 0);
  setPos(mlHip, 0);
  setPos(blHip, 0);
  setPos(frHip, 0);
  setPos(mrHip, 0);
  setPos(brHip, 0);

  delay(500);
  
  setPos(mrKnee, 0);
  setPos(mlKnee, 0);
  
  standing = false;
  delay(500);
}

void stopMoving() {
  pwm.setPWM(flHip.getServoNum(), 0, 0);
  pwm.setPWM(frHip.getServoNum(), 0, 0);
  pwm.setPWM(mlHip.getServoNum(), 0, 0);
  pwm.setPWM(mrHip.getServoNum(), 0, 0);
  pwm.setPWM(blHip.getServoNum(), 0, 0);
  pwm.setPWM(brHip.getServoNum(), 0, 0);
  
  pwm.setPWM(flKnee.getServoNum(), 0, 0);
  pwm.setPWM(frKnee.getServoNum(), 0, 0);
  pwm.setPWM(mlKnee.getServoNum(), 0, 0);
  pwm.setPWM(mrKnee.getServoNum(), 0, 0);
  pwm.setPWM(blKnee.getServoNum(), 0, 0);
  pwm.setPWM(brKnee.getServoNum(), 0, 0);
}

void setDeg(HexJoint joint, int deg) {
  if (joint.getDeg() == deg) {
    return;
  }

  joint.setDeg(deg);
  pwm.setPWM(joint.getServoNum(), 0, pulseWidth(deg));
}

void setPos(HexJoint joint, int pos) {

  int pos0;
  int pos1;
  int pos2;
  if (joint.isVertical()) {
    pos0 = 80;
    pos1 = 60;
    pos2 = 1;
  } else {
    pos0 = 90;
    pos1 = 130;
    pos2 = 50;
  }
  
  int deg = pos0;
  
  switch(pos) {
    case 0:
      deg = pos0;
      break;
    case 1:
      deg = pos1;
      break;
    case 2:
      deg = pos2;
      break;
  }

  if (joint.isInverted()) {
    deg = 180 - deg;
  }

  if (joint.getDeg() == deg) {
    return;
  }

  joint.setDeg(deg);
  pwm.setPWM(joint.getServoNum(), 0, pulseWidth(deg));
}
