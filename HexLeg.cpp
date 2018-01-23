#include "HexLeg.h"

HexLeg::HexLeg() {
  
}

void HexLeg::begin(HexJoint hip, HexJoint knee) {
  this->hip = hip;
  this->knee = knee;
}
