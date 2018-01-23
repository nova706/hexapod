#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "HexJoint.h"
#include "HexLeg.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define FREQUENCY 50

#define SERVOMIN    650 // 860 // spec: 500
#define SERVOMAX    2350 // 2140 // spec: 2400

#define IDLE        0
#define STAND       1
#define SIT         2
#define WALK_F      3
#define WALK_B      4
#define TURN_R      5
#define TURN_L      6
#define STRAFE_R    7
#define STRAFE_L    8

int action = IDLE;
int speed = 150;

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

HexLeg fl = HexLeg();
HexLeg ml = HexLeg();
HexLeg bl = HexLeg();
HexLeg fr = HexLeg();
HexLeg mr = HexLeg();
HexLeg br = HexLeg();

int serialData = 0;// data and serial data are just variables to hold information.
int data = 0;

void setup() 
{
  Serial.begin(9600);

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

  fl.begin(flHip, flKnee);
  ml.begin(mlHip, mlKnee);
  bl.begin(blHip, blKnee);
  fr.begin(frHip, frKnee);
  mr.begin(mrHip, mrKnee);
  br.begin(brHip, brKnee);

  stand();
}

void loop() {
  if (Serial.available() > 0) { //Serial.available will tell the board to look at the information being sent to it through the serial port.
    data = Serial.read(); //The arduino reads this data and writes it as one of our variables.
    Serial.println(data); //The arduino prints this data to the serial monitor so we can see what it sees.
  }

  if (data == 53) { // Keypad '5'
    if (action == STAND) {
      sit();
    } else if (action == SIT) {
      stand();
    } else {
      idle();
    }
    data = 0; // Do not call idle/sit/stand in a loop
  } else if (data == 56) { // Keypad '8'
    walk(1);
  } else if (data == 50) { // Keypad '2'
    walk(-1);
  } else if (data == 57) { // Keypad '9'
    turn(1);
  } else if (data == 55) { // Keypad '7'
    turn(-1);
  } else if (data == 54) { // Keypad '6'
    strafe(1);
  } else if (data == 52) { // Keypad '4'
    strafe(-1);
  }
}

void idle() {
  Serial.println("idle");

  action = IDLE;

  stepTo(fl, 0, speed);
  stepTo(bl, 0, speed);
  stepTo(mr, 0, speed);
  stepTo(ml, 0, speed);
  stepTo(fr, 0, speed);
  stepTo(br, 0, speed);

  stopMoving();
}

void stand() {
  Serial.println("stand");

  action = STAND;

  setPos(flHip, 0);
  setPos(mlHip, 0);
  setPos(blHip, 0);
  setPos(frHip, 0);
  setPos(mrHip, 0);
  setPos(brHip, 0);
  waitForServo(speed);

  setPos(flKnee, 0);
  setPos(mlKnee, 0);
  setPos(blKnee, 0);
  setPos(frKnee, 0);
  setPos(mrKnee, 0);
  setPos(brKnee, 0);
  waitForServo(speed);
  
  stopMoving();
}

void sit() {
  Serial.println("sit");

  action = SIT;
  
  setPos(flKnee, 2);
  setPos(mlKnee, 2);
  setPos(blKnee, 2);
  setPos(frKnee, 2);
  setPos(mrKnee, 2);
  setPos(brKnee, 2);
  waitForServo(speed);

  setPos(flHip, 0);
  setPos(mlHip, 0);
  setPos(blHip, 0);
  setPos(frHip, 0);
  setPos(mrHip, 0);
  setPos(brHip, 0);
  waitForServo(speed);
  
  stopMoving();
}

void walk(int dir) {
  Serial.println("walk");

  int dirPos = 1;
  if (dir == -1) {
    dirPos = 2;
    action = WALK_B;
  } else {
    action = WALK_F;
  }

  stepTo(fl, dirPos, speed);
  stepTo(bl, dirPos, speed);
  stepTo(mr, dirPos, speed);

  stepTo(ml, dirPos, speed);
  stepTo(fr, dirPos, speed);
  stepTo(br, dirPos, speed);

  setPos(flHip, 0);
  setPos(mlHip, 0);
  setPos(blHip, 0);
  setPos(frHip, 0);
  setPos(mrHip, 0);
  setPos(brHip, 0);
  waitForServo(speed);
}

void turn(int dir) {
  Serial.println("turn");

  int forwardPos = 1;
  int backwardPos = 2;
  if (dir == -1) {
    forwardPos = 2;
    backwardPos = 1;
    action = TURN_L;
  } else {
    action = TURN_R;
  }

  stepTo(fl, forwardPos, speed);
  stepTo(bl, forwardPos, speed);
  stepTo(mr, backwardPos, speed);

  stepTo(ml, forwardPos, speed);
  stepTo(fr, backwardPos, speed);
  stepTo(br, backwardPos, speed);

  setPos(flHip, 0);
  setPos(mlHip, 0);
  setPos(blHip, 0);
  setPos(frHip, 0);
  setPos(mrHip, 0);
  setPos(brHip, 0);
  waitForServo(speed);
}

void strafe(int dir) {
  Serial.println("strafe");

  int forwardPos = 1;
  int backwardPos = 2;
  if (dir == -1) {
    forwardPos = 2;
    backwardPos = 1;
    action = STRAFE_L;
  } else {
    action = STRAFE_R;
  }

  stepTo(fl, forwardPos, speed);
  stepTo(br, forwardPos, speed);

  stepTo(fr, backwardPos, speed);
  stepTo(bl, backwardPos, speed);

  // Lift middle knees before moving the body to the new position.
  setPos(mrKnee, 1);
  setPos(mlKnee, 1);
  waitForServo(speed);

  setPos(flHip, 0);
  setPos(mlHip, 0);
  setPos(blHip, 0);
  setPos(frHip, 0);
  setPos(mrHip, 0);
  setPos(brHip, 0);
  waitForServo(speed);

  // Lower the middle knees back down.
  setPos(mrKnee, 0);
  setPos(mlKnee, 0);
  waitForServo(speed);
}

void getWalkStack(int gate) {

  // L: Lift
  // R: Rotate
  // P: Place

  // TODO: Fix angles to ensure that when rotating two legs towards each other, they do not touch.

  // GATE 0 WALK: Two sets of three legs move together. When one set is lifted, the other set moves the body forward.
  // fl       ml       bl       fr       mr       br
  // L                 L                 L         
  // R+       R-       R+       R-       R+       R-
  // P                 P                 P         
  //          L                 L                 L
  // R-       R+       R-       R+       R-       R+
  //          P                 P                 P

  // GATE 1 LUNGE: Two sets of three legs move together. First position both sets, then move the body together at once.
  // fl       ml       bl       fr       mr       br
  // L                 L                 L         
  // R+                R+                R+        
  // P                 P                 P         
  //          L                 L                 L
  //          R+                R+                R+
  //          P                 P                 P
  // R-       R-       R-       R-       R-       R-

  // GATE 2 RIPPLE: Three sets of two legs move together. When one set is lifted, the other two sets move the body forward. Back to front.
  // fl       ml       bl       fr       mr       br
  //                   L                          L
  // R-       R-       R++      R-       R-       R++
  //                   P                          P
  //          L                          L        
  // R-       R++      R-       R-       R++      R-
  //          P                          P          
  // L                          L        
  // R++      R-       R-       R++      R-       R-
  // P                          P          
  
}

void waitForServo(int milliseconds) {
  delay(milliseconds);
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

void stepTo(HexLeg leg, int pos, int speed) {
  int targetHipDeg = getPosDeg(leg.hip, pos);

  if (leg.hip.getDeg() == targetHipDeg) {
    return;
  }

  // TODO: Interpolate the step

  setPos(leg.knee, 1); // Lift leg
  waitForServo(speed);
  setDeg(leg.hip, targetHipDeg); // Move hip to target
  waitForServo(speed);
  setPos(leg.knee, 0); // Set leg down
  waitForServo(speed);
}

void setDeg(HexJoint joint, int deg) {
  if (joint.getDeg() == deg) {
    return;
  }

  joint.setDeg(deg);
  pwm.setPWM(joint.getServoNum(), 0, pulseWidth(deg));
}

void setPos(HexJoint joint, int pos) {
  int deg = getPosDeg(joint, pos);
  setDeg(joint, deg);
}

int getPosDeg(HexJoint joint, int pos) {
  int pos0;
  int pos1;
  int pos2;
  if (joint.isVertical()) {
    pos0 = 80;
    pos1 = 50;
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

  return deg;
}

int pulseWidth(int angle) {
  Serial.println(angle);

  int pulse_wide, analog_value;
  pulse_wide = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  analog_value = (float(pulse_wide) * FREQUENCY * 4096) / 1000000;
  return analog_value;
}
