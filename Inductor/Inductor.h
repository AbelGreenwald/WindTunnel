#include "mbed.h"

class Inductor {
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