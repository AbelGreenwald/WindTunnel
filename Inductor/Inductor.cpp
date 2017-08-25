#include "mbed.h"
#include "PID.h"
#include "Inductor.h"

Inductor::Inductor(PinName in1, PinName in2, PwmOut enA) : PID () {

	DigitalOut in1_(in1);
	DigitalOut in2_(in2);
	PwmOut     enA_(enA);

    this->setInputLimits(0.0, 1.0);
    this->setOutputLimits(0.0, 1.0);
    this->setTunings(Kc, tauI, tauD);
}

void Inductor::update( long pVal ) {
//    pid.setProcessValue( pVal );
    pwr = this->compute();

    while(true) {
//        pid.setProcessValue(val1);
//        power_val = pid.compute();
       if (pwr >= (float)0) {
//            in1_ = 1;
//            in2_ = 0;
//            enA_ = pwr;
        }
        else {
//            in1_.write(0);
//            in2_.write(1);
//            enA_.write(pwr * -1);
        }
    }

}