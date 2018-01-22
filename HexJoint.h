#ifndef _HexJoint_H
#define _HexJoint_H

class HexJoint {
  public:
    HexJoint(void);
	void begin(int servoNum, bool vertical, bool invert);
    void setDeg(int deg);
	int getDeg(void);
	int getServoNum(void);
	bool isVertical(void);
	bool isInverted(void);
  
  private:
    int servoNum;
    int deg;
	bool vertical;
	bool invert;
};

#endif
