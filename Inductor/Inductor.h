#include "mbed.h"
#include "PID.h"

#ifndef Inductor_H
#define Inductor_H

class Inductor : public PID {

public:
	Inductor(PinName in1, PinName in2, PwmOut enA);
	void update( long pVal );

protected:
	float pwr;
	float Kc, tauI, tauD, interval;
	DigitalOut in1_();
	DigitalOut in2_();
	PwmOut     enA_();
};

#endif