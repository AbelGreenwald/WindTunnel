#include "mbed.h"
#include "PID.h"

#ifndef Inductor_H
#define Inductor_H

class Inductor : public PID, PwmOut, DigitalOut {
public:
	Inductor(PinName enA, PinName in);
	void Initialize();
	void update();
	void setPolarity(bool pol);
	bool getPolarity();
	void setProtection(bool protect);
	bool getProtection();
	void setPower(float pwr);
	float getPower();
protected:
    bool protection_, polarity_, power_;
};

#endif