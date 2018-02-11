#ifndef _TargetPosition_H
#define _TargetPosition_H

#include "HexJoint.h"
#include "HexLeg.h"

class TargetPosition {
  public:
    TargetPosition(void);
    TargetPosition(HexJoint joint, int pos);
    TargetPosition(HexLeg leg, int pos);
    HexJoint joint;
    HexLeg leg;
    bool isLeg;
    int pos;
};

#endif
