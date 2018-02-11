#include "TargetPosition.h"

TargetPosition::TargetPosition() {
}

TargetPosition::TargetPosition(HexJoint joint, int pos) {
  this->joint = joint;
  this->pos = pos;
  this->isLeg = false;
}

TargetPosition::TargetPosition(HexLeg leg, int pos) {
  this->leg = leg;
  this->pos = pos;
  this->isLeg = true;
}
