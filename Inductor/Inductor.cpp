#include "mbed.h"
#include "PID.h"
#include "Inductor.h"

Inductor::Inductor(PinName enA, PinName polarity) : PID (), PwmOut(enA), DigitalOut(polarity) {

    //Defaults for PWM
    PwmOut::write(.25);

    //Defaults for DigitalOut
    DigitalOut::write(0);
    
    //Defaults for PID
    usingFeedForward       = false;
    inAuto                 = false;
    setPoint_              = 0.0;
    processVariable_       = 0.0;
    prevProcessVariable_   = 0.0;
    controllerOutput_      = 0.0;
    prevControllerOutput_  = 0.0;
    accError_              = 0.0;
    bias_                  = 0.0;
    realOutput_            = 0.0;
    tSample_               =   1; //not zero

    setInterval(.01);
    setInputLimits (0.0, 1.0);
    setOutputLimits(0.0, 1.0);
    setTunings(1.0, 1.0, 1.0);

    //Temporary things
    protection_ = false;
//    period_ms(6);
}

void Inductor::Initialize() {
;

}

void Inductor::update() {

}

float Inductor::getPower() {
return power_;
}

void Inductor::setPower(float pwr) {

    PwmOut::write(pwr);

}

bool Inductor::getPolarity() {
return (bool)DigitalOut::read();
}

void Inductor::setPolarity(bool polarity) {

    polarity_ = polarity;

    if (polarity_ == true) {
        DigitalOut::write(1);
    }
    else {
        DigitalOut::write(0);
    }
}

bool Inductor::getProtection() {
    return protection_;
}

void Inductor::setProtection(bool protect) {
    protection_ = protect;
}
