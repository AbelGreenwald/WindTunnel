#include "mbed.h"
#include "Inductor.h"
#include "PID.h"

Inductor::Inductor(PinName in1, PinName in2, PwmOut enA) {

    float pwr = 0;
    DigitalOut in1_(in1);
//    PID pid(Kc, tauI, tauD, interval);
}

void Inductor::update( long pVal ) {
//    pid.setProcessValue( pVal );
//    pwr = compute();

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