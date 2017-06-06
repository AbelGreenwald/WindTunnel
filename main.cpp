#include "mbed.h"
#include "Servo.h"
#include "cmsis_os.h"
#include "mag3110.h"
#include "PID.h"
// target X position (teslas) for magnometer
#define targetX 0
//#define AUTO_MODE 1
/* Set Pins */
// power on blinking LED
DigitalOut led1(LED1);

PwmOut     enA(D9);
DigitalOut in1(D8);
DigitalOut in2(D7);

//motor potentiometer
AnalogIn pot1(A5);
AnalogIn tmp(A4);
// motor ESC
Servo servo(D3);
// Timer inturrupt for power LED
Ticker timer;
// inductor1 pwm
PID pidX1(.000825,15,2,.01);
//PwmOut      inductor1(PB_3);
// delta timer for calculations
Timer timeTimer;
// state output and debug info
Serial pc(USBTX, USBRX, 115200);
// magnometer i2c
//MAG3110 mag = MAG3110(D14, D15);
float mag_val, power_val;
volatile int val1;
volatile int val2;
volatile int val3;

//float * magX;
//const float inductor1[3][3] = [ 0,0,0; 0,1,0; 0,0,1 ];
//const float inductor2[3][3] = [ 0,1,0; 0,0,1; 1,0,0 ];
//const float inductor3[3][3] = [ 0,0,1; 1,0,0; 0,1,0 ];
//const float inductor4[3][3] = [ 0,0,1; 1,0,0; 0,1,0 ];
//const float inductor5[3][3] = [ 0,0,1; 1,0,0; 0,1,0 ];
//const float inductor6[3][3] = [ 0,0,1; 1,0,0; 0,1,0 ];
// delta magnometer X
//float delMagX;

void motor_thread(void const *argument)
{
    while (true) {
           // power_val = pot1.read();
            servo = power_val;
            osDelay(50);
    }
}

void inductor_X1_thread(void const *argument)
{
    in1 = 1;
    in2 = 0;
    pidX1.setInputLimits(-2000, 400);
    pidX1.setOutputLimits(0.0,1.0);
    pidX1.setSetPoint(-850.0);
    pidX1.setMode(AUTO_MODE);
    
    while(true)
    {
    pidX1.setProcessValue(val1);
    power_val = pidX1.compute();
    enA = power_val;
    }
}

void blink() {
    led1 = !led1;
}

void mag_reading(void const *argument)
{
    MAG3110 mag(D14, D15);
    float tmp_val;
    wait_ms(100);
    mag.enable();
    wait_ms(100);
    while(true){
            val1 = mag.getMagAxis(MAG3110_X_AXIS);
            tmp_val = 100 * tmp.read() * 4.5 - 50;
            osDelay(50);
            pc.printf("mag: %4.0i uT, ",val1);
            pc.printf("tmp: %2.0f C, ",tmp_val);
            pc.printf("Pwr: %0.5f \r\n",(power_val));
    }
}

osThreadDef(motor_thread, osPriorityNormal, DEFAULT_STACK_SIZE);
osThreadDef(mag_reading, osPriorityNormal, DEFAULT_STACK_SIZE);
osThreadDef(inductor_X1_thread, osPriorityNormal, DEFAULT_STACK_SIZE);

int main()
{
    timer.attach(&blink, .5);
    osThreadCreate(osThread(motor_thread), NULL);
    osThreadCreate(osThread(mag_reading), NULL);
    osThreadCreate(osThread(inductor_X1_thread), NULL);

    while (true) {
 //           pc.printf("measurement: %f; mag_reading: %f\r", power_val, *magX);
            osDelay(50);
            }

}