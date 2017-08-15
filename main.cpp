#include "mbed.h"
#include "Servo.h"
#include "cmsis_os.h"
#include "mag3110.h"
#include "PID.h"
#include "main.h"
#include "Inductor.h"

osThreadDef(motor_thread, osPriorityNormal, DEFAULT_STACK_SIZE);
osThreadDef(mag_reading, osPriorityNormal, DEFAULT_STACK_SIZE);
osThreadDef(inductor_X1_thread, osPriorityNormal, DEFAULT_STACK_SIZE);

void motor_thread(void const *argument) {
    while (true) {
           // power_val = pot1.read();
            tunnel_motor = power_val;
            osDelay(50);
    }
}

//Inductor Constructor
// get clock from global
// Process Value
// Uses constants
// - Input Min
// - Input Max
// - Set Point
void inductor_X1_thread(void const *argument) {

//    inductor_X1_pid.setInputLimits(-3250, 200);
//    inductor_X1_pid.setOutputLimits(-1,1);
//    inductor_X1_pid.setSetPoint(-2000);
//    inductor_X1_pid.setMode(AUTO_MODE);
//Inductor X_l(inductor_X1_in1, inductor_X1_in2, inductor_X1_enA)

Inductor X_l(D8, D9, inductor_X_l_enA);

}

void mag_reading(void const *argument) {
    MAG3110 mag(D14, D15);
    float tmp_val;
    wait_ms(100);
    mag.enable();
    wait_ms(100);
    while(true){
            val1 = mag.getMagAxis(MAG3110_X_AXIS);
            tmp_val = 100 * inductor_X1_tmp.read() * 4.5 - 50;
            osDelay(50);
            pc.printf("mag: %4.0i uT, ",val1);
            pc.printf("tmp: %2.0f C, ",tmp_val);
            pc.printf("Pwr: %0.5f \r\n",(power_val));
    }
}

int main() {
    timer.attach(&blink, .5);
    osThreadCreate(osThread(motor_thread), NULL);
    osThreadCreate(osThread(mag_reading), NULL);
    osThreadCreate(osThread(inductor_X1_thread), NULL);

    while (true) {
        osDelay(100);
    }
}

void blink() {
    led1 = !led1;
}