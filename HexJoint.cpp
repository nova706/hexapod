#include <HexJoint.h>

HexJoint::HexJoint() {
  
}

void HexJoint::begin(int servoNum, bool vertical, bool invert) {
  this->servoNum = servoNum;
  this->vertical = vertical;
  this->invert = invert;
}

int HexJoint::getDeg(void) {
	return deg;
}

void HexJoint::setDeg(int deg) {
  this->deg = deg;
}

int HexJoint::getServoNum(void) {
  return servoNum;
}

bool HexJoint::isVertical(void) {
  return vertical;
}

bool HexJoint::isInverted(void) {
  return invert;
}