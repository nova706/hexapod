#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "HexJoint.h"
#include "HexLeg.h"
#include "TargetPosition.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define FREQUENCY 50

#define SERVOMIN    650 // 860 // spec: 500
#define SERVOMAX    2350 // 2140 // spec: 2400

#define STAND       0
#define SIT         1
#define WALK_F      2
#define WALK_B      3
#define TURN_R      4
#define TURN_L      5
#define STRAFE_R    6
#define STRAFE_L    7

int action = STAND;
int speed = 200;
TargetPosition targets[5][6];
int targetSize = 0;
int targetIndex = 0;
bool loopTargets = false;

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
      // If pressed during any other motion stop the motion and go to stand
      stepToStand();
    }
  } else if (data == 56) { // Keypad '8'
    setWalkTargets(0, 1);
  } else if (data == 50) { // Keypad '2'
    setWalkTargets(0, -1);
  } else if (data == 57) { // Keypad '9'
    setTurnTargets(0, 1);
  } else if (data == 55) { // Keypad '7'
    setTurnTargets(0, -1);
  } else if (data == 54) { // Keypad '6'
    setStrafeTargets(0, 1);
  } else if (data == 52) { // Keypad '4'
    setStrafeTargets(0, -1);
  }

  data = 0; // Clear serial data after setting targets.

  advanceTarget();
}

void advanceTarget() {
  if (targetSize > 0) {

    Serial.println("target index");
    Serial.println(targetIndex);

    // For each joint in the target index, set the position
    int i;
    for (i = 0; i < 6; i++) {
      TargetPosition target = targets[targetIndex][i];

      if (!target.isLeg) {
        setPos(target.joint, target.pos);
      } else {
        stepTo(target.leg, target.pos, speed);
      }
    }

    waitForServo(speed);

    // Increase target index. If target index is the size of the number of targets, set the index back to 0.
    targetIndex = targetIndex + 1;
    if (targetIndex == targetSize) {
      if (loopTargets) {
        targetIndex = 0;
        Serial.println("looping");
      } else {
        stopMoving();
        resetTargets();
      }
    }
  }
}

void resetTargets() {
  loopTargets = false;
  targetSize = 0;
  targetIndex = 0;
}

void stand() {
  Serial.println("stand");

  action = STAND;
  loopTargets = false;

  // L: Lift
  // R: Rotate
  // P: Place
  // S: Step

  // STAND: Center all hips, then lower all knees.
  // fl       ml       bl       fr       mr       br
  // R        R        R        R        R        R 
  // L        L        L        L        L        L

  targetSize = 2;
  setTargetsStep(0, TargetPosition(fl.hip, 0), TargetPosition(ml.hip, 0), TargetPosition(bl.hip, 0), TargetPosition(fr.hip, 0), TargetPosition(mr.hip, 0), TargetPosition(br.hip, 0));
  setTargetsStep(1, TargetPosition(fl.knee, 0), TargetPosition(ml.knee, 0), TargetPosition(bl.knee, 0), TargetPosition(fr.knee, 0), TargetPosition(mr.knee, 0), TargetPosition(br.knee, 0));
}

void stepToStand() {
  Serial.println("stepToStand");

  action = STAND;
  loopTargets = false;

  // L: Lift
  // R: Rotate
  // P: Place
  // S: Step

  // STAND: Center all hips, then lower all knees.
  // fl       ml       bl       fr       mr       br
  // S                 S                 S          
  //          S                 S                 S

  targetSize = 2;
  setTargetsStep(0, TargetPosition(fl, 0), TargetPosition(ml.knee, 0), TargetPosition(bl, 0), TargetPosition(fr.knee, 0), TargetPosition(mr, 0), TargetPosition(br.knee, 0));
  setTargetsStep(1, TargetPosition(fl.knee, 0), TargetPosition(ml, 0), TargetPosition(bl.knee, 0), TargetPosition(fr, 0), TargetPosition(mr.knee, 0), TargetPosition(br, 0));
}

void sit() {
  Serial.println("sit");

  action = SIT;
  loopTargets = false;

  // L: Lift
  // R: Rotate
  // P: Place
  // S: Step

  // SIT: Raise all knees, then center hips.
  // fl       ml       bl       fr       mr       br
  // L        L        L        L        L        L
  // R        R        R        R        R        R 

  targetSize = 2;
  setTargetsStep(0, TargetPosition(fl.knee, -1), TargetPosition(ml.knee, -1), TargetPosition(bl.knee, -1), TargetPosition(fr.knee, -1), TargetPosition(mr.knee, -1), TargetPosition(br.knee, -1));
  setTargetsStep(1, TargetPosition(fl.hip, 0), TargetPosition(ml.hip, 0), TargetPosition(bl.hip, 0), TargetPosition(fr.hip, 0), TargetPosition(mr.hip, 0), TargetPosition(br.hip, 0));
}

void setWalkTargets(int gate, int dir) {

  int forwardPos = 1;
  int backwardPos = -1;
  if (dir == -1) {
    forwardPos = -1;
    backwardPos = 1;
    action = WALK_B;
  } else {
    action = WALK_F;
  }

  loopTargets = true;

  // L: Lift
  // R: Rotate
  // P: Place
  // S: Step

  // TODO: Fix angles to ensure that when rotating two legs towards each other, they do not touch.

  if (gate == 0) {
    
    // GATE 0 WALK: Two sets of three legs move together. When one set is lifted, the other set moves the body forward.
    // fl       ml       bl       fr       mr       br
    // S+       R-       S+       R-       S+       R-
    // R-       S+       R-       S+       R-       S+

    targetSize = 2;
    setTargetsStep(0, TargetPosition(fl, forwardPos), TargetPosition(ml.hip, backwardPos), TargetPosition(bl, forwardPos), TargetPosition(fr.hip, backwardPos), TargetPosition(mr, forwardPos), TargetPosition(br.hip, backwardPos));
    setTargetsStep(1, TargetPosition(fl.hip, backwardPos), TargetPosition(ml, forwardPos), TargetPosition(bl.hip, backwardPos), TargetPosition(fr, forwardPos), TargetPosition(mr.hip, backwardPos), TargetPosition(br, forwardPos));

  } else if (gate == 1) {

    // GATE 1 LUNGE: Two sets of three legs move together. First position both sets, then move the body together at once.
    // fl       ml       bl       fr       mr       br
    // S+                S+                S+        
    //          S+                S+                S+
    // R-       R-       R-       R-       R-       R-

    // Can this be reduced to combine steps 3/4 (so that placing and lifting are in one step)?

    targetSize = 3;
    setTargetsStep(0, TargetPosition(fl, forwardPos), TargetPosition(ml.knee, 0), TargetPosition(bl, forwardPos), TargetPosition(fr.knee, 0), TargetPosition(mr, forwardPos), TargetPosition(br.knee, 0));
    setTargetsStep(1, TargetPosition(fl.knee, 0), TargetPosition(ml, forwardPos), TargetPosition(bl.knee, 0), TargetPosition(fr, forwardPos), TargetPosition(mr.knee, 0), TargetPosition(br, forwardPos));
    setTargetsStep(2, TargetPosition(fl.hip, backwardPos), TargetPosition(ml.hip, backwardPos), TargetPosition(bl.hip, backwardPos), TargetPosition(fr.hip, backwardPos), TargetPosition(mr.hip, backwardPos), TargetPosition(br.hip, backwardPos));
    
  } else if (gate == 2) {

    // GATE 2 RIPPLE: Three sets of two legs move together. When one set is lifted, the other two sets move the body forward. Back to front.
    // fl       ml       bl       fr       mr       br
    // R        R-       S+       R        R-       S+ 
    // R-       S+       R        R-       S+       R 
    // S+       R        R-       S+       R        R-

    targetSize = 3;
    setTargetsStep(0, TargetPosition(fl.hip, 0), TargetPosition(ml.hip, backwardPos), TargetPosition(bl, forwardPos), TargetPosition(fr.hip, 0), TargetPosition(mr.hip, backwardPos), TargetPosition(br, forwardPos));
    setTargetsStep(1, TargetPosition(fl.hip, backwardPos), TargetPosition(ml, forwardPos), TargetPosition(bl.hip, 0), TargetPosition(fr.hip, backwardPos), TargetPosition(mr, forwardPos), TargetPosition(br.hip, 0));
    setTargetsStep(2, TargetPosition(fl, forwardPos), TargetPosition(ml.hip, 0), TargetPosition(bl.hip, backwardPos), TargetPosition(fr, forwardPos), TargetPosition(mr.hip, 0), TargetPosition(br.hip, backwardPos));
  
  }
}

void setTurnTargets(int gate, int dir) {

  int forwardPos = 1;
  int backwardPos = -1;
  if (dir == -1) {
    forwardPos = -1;
    backwardPos = 1;
    action = TURN_L;
  } else {
    action = TURN_R;
  }

  loopTargets = true;

  // L: Lift
  // R: Rotate
  // P: Place
  // S: Step

  // TODO: Fix angles to ensure that when rotating two legs towards each other, they do not touch.

  if (gate == 0) {
    
    // GATE 0 TURN: Two sets of three legs move together. When one set is lifted, the other set rotates the body.
    // fl       ml       bl       fr       mr       br
    // S+       R-       S+       R+       S-       R+
    // R-       S+       R-       S-       R+       S-

    targetSize = 2;
    setTargetsStep(0, TargetPosition(fl, forwardPos), TargetPosition(ml.hip, backwardPos), TargetPosition(bl, forwardPos), TargetPosition(fr.hip, forwardPos), TargetPosition(mr, backwardPos), TargetPosition(br.hip, forwardPos));
    setTargetsStep(1, TargetPosition(fl.hip, backwardPos), TargetPosition(ml, forwardPos), TargetPosition(bl.hip, backwardPos), TargetPosition(fr, backwardPos), TargetPosition(mr.hip, forwardPos), TargetPosition(br, backwardPos));
    
  }
}

void setStrafeTargets(int gate, int dir) {

  int forwardPos = 1;
  int backwardPos = -1;
  if (dir == -1) {
    forwardPos = -1;
    backwardPos = 1;
    action = TURN_L;
  } else {
    action = TURN_R;
  }

  loopTargets = true;

  // L: Lift
  // R: Rotate
  // P: Place
  // S: Step

  // TODO: Fix angles to ensure that when rotating two legs towards each other, they do not touch.

  if (gate == 0) {
    
    // GATE 0 STRAFE: Two sets of two legs move into position. Middle legs are lifted and the two sets of two translate the body horizontally.
    // fl       ml       bl       fr       mr       br
    // S+                S-                                               
    //                            S+                S-
    //          L                          L         
    // R-                R+       R-                R+
    //          P                          P

    targetSize = 5;
    setTargetsStep(0, TargetPosition(fl, forwardPos), TargetPosition(ml.knee, 0), TargetPosition(bl, backwardPos), TargetPosition(fr.knee, 0), TargetPosition(mr, 0), TargetPosition(br.knee, 0));
    setTargetsStep(1, TargetPosition(fl.knee, 0), TargetPosition(ml.knee, 0), TargetPosition(bl.knee, 0), TargetPosition(fr, forwardPos), TargetPosition(mr.knee, 0), TargetPosition(br, backwardPos));
    setTargetsStep(2, TargetPosition(fl.knee, 0), TargetPosition(ml.knee, 1), TargetPosition(bl.knee, 0), TargetPosition(fr.knee, 0), TargetPosition(mr.knee, 1), TargetPosition(br.knee, 0));
    setTargetsStep(3, TargetPosition(fl.hip, backwardPos), TargetPosition(ml.knee, 1), TargetPosition(bl.hip, forwardPos), TargetPosition(fr.hip, backwardPos), TargetPosition(mr.knee, 1), TargetPosition(br.hip, forwardPos));
    setTargetsStep(4, TargetPosition(fl.knee, 0), TargetPosition(ml.knee, 0), TargetPosition(bl.knee, 0), TargetPosition(fr.knee, 0), TargetPosition(mr.knee, 0), TargetPosition(br.knee, 0));
  }
}

void setTargetsStep(int index, TargetPosition t0, TargetPosition t1, TargetPosition t2, TargetPosition t3, TargetPosition t4, TargetPosition t5) {
  targets[index][0] = t0;
  targets[index][1] = t1;
  targets[index][2] = t2;
  targets[index][3] = t3;
  targets[index][4] = t4;
  targets[index][5] = t5;
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
  int posN;
  if (joint.isVertical()) {
    pos0 = 80;
    pos1 = 50;
    posN = 1;
  } else {
    pos0 = 90;
    pos1 = 130;
    posN = 50;
  }
  
  int deg = pos0;
  
  switch(pos) {
    case 0:
      deg = pos0;
      break;
    case 1:
      deg = pos1;
      break;
    case -1:
      deg = posN;
      break;
  }

  if (joint.isInverted()) {
    deg = 180 - deg;
  }

  return deg;
}

int pulseWidth(int angle) {
  //Serial.println(angle);

  int pulse_wide, analog_value;
  pulse_wide = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  analog_value = (float(pulse_wide) * FREQUENCY * 4096) / 1000000;
  return analog_value;
}
