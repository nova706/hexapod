#ifndef _HexLeg_H
#define _HexLeg_H

#include "HexJoint.h"

class HexLeg {
  public:
    HexLeg(void);
    void begin(HexJoint hip, HexJoint knee);
    HexJoint hip;
    HexJoint knee;
};

#endif
