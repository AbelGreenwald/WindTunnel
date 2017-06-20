#include "mbed.h"
#include "Servo.h"
#include "cmsis_os.h"
#include "mag3110.h"
#include "PID.h"

// target X position (teslas) for magnometer
#define targetX 0
#define BLUE    "\e[1;34m"
#define NONE    "\033[0m"

/* Set Digital Pins */
// power on blinking LED
DigitalOut led1(LED1);
DigitalOut in1(D8);
DigitalOut in2(D7);

InterruptIn Switch_1(D4);

/* Set Analoge Pins */
//motor potentiometer
AnalogIn pot1(A5);
AnalogIn tmp(A4);

PwmOut enA(D9);
Servo  servo(D3);

/* Set Other Things*/
Ticker timer;

volatile int val1;
volatile int val2;
volatile int val3;
volatile float tmp_val;
float Kc = 0;
float tauI = 0; 
float tauD = 0;
bool debounce = false;
char* Kc_color = "\e[1;34m";
char* tauI_color = "\e[1;34m";
char* tauD_color = "\e[1;39m";
unsigned short active_parameter = 1;

// state output and debug info
Serial pc(USBTX, USBRX, 115200);
// magnometer i2c

//MAG3110 mag = MAG3110(D14, D15);
float mag_val, power_val;

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
    PID pidX1(Kc,tauI,tauD,.01);

    in1 = 1;
    in2 = 0;
    pidX1.setInputLimits(-2000, 400);
    pidX1.setOutputLimits(0.0,1.0);
    pidX1.setSetPoint(-1450.0);
    pidX1.setMode(AUTO_MODE);
    
    while(true)
    {
    pidX1.setProcessValue(val1);
    pidX1.setTunings(Kc, tauI, tauD);
    //reverse the direction (in1, in2) here
    power_val = pidX1.compute();
    enA = power_val;
    }
}

// blink ISR
void blink() {
    led1 = !led1;
    //clear button int
    debounce = false;
}

// push_event ISR
void push_event() {
    //change between K's and change print color
    //TODO: debounce better
    if (!debounce)
    {
    debounce = true;
    switch ( active_parameter ) {
    case 1:
        Kc_color = "\e[1;39m";
        tauI_color = "\e[1;34m";
        tauD_color = "\e[1;39m";
        Kc = pot1.read() * 10;
        active_parameter = 2;
        break;
    case 2:
        Kc_color = "\e[1;39m";
        tauI_color = "\e[1;39m";
        tauD_color = "\e[1;34m";
        tauI = pot1.read() * 10;
        active_parameter = 3;
        break;
    case 3:
        Kc_color = "\e[1;34m";
        tauI_color = "\e[1;39m";
        tauD_color = "\e[1;39m";
        tauD = pot1.read() * 10;
        active_parameter = 1;
        break;
        }
    }
}

void mag_reading(void const *argument)
{
    MAG3110 mag(D14, D15);
    wait_ms(100);
    mag.enable();
    wait_ms(100);

    while(true){
        if (mag.dataReady() == 255 )
        {
            val1 = mag.getMagAxis(MAG3110_X_AXIS);
            tmp_val = 100 * tmp.read() * 4.5 - 50;
        }
    }
}

void print_data_thread(void const *argument)
{   
    osDelay(50);
    pc.printf("mag (uT),");
    pc.printf("tmp (C),");
    pc.printf("Pwr (%%)");
    pc.printf("\t");
    pc.printf("Kc,");
    pc.printf("tauI,");
    pc.printf("tauD");
    pc.printf("\r\n");
    while(true){
        osDelay(250);
        pc.printf("%4.0i,",val1);
        pc.printf("%2.0f,",tmp_val);
        pc.printf("%0.5f",power_val);
        pc.printf("\t");
        pc.printf("%s%f," NONE,Kc_color,Kc);
        pc.printf("%s%f," NONE,tauI_color,tauI);
        pc.printf("%s%f" NONE,tauD_color,tauD);
        pc.printf("\r\n");
    }
}

// Define Threads and Priorities to Run
osThreadDef(motor_thread, osPriorityNormal, DEFAULT_STACK_SIZE);
osThreadDef(mag_reading, osPriorityNormal, DEFAULT_STACK_SIZE);
osThreadDef(print_data_thread, osPriorityNormal, DEFAULT_STACK_SIZE);
osThreadDef(inductor_X1_thread, osPriorityNormal, DEFAULT_STACK_SIZE);

int main()
{
    // Set blinking LED timer Inturrupt
    timer.attach(&blink, .5);
    // Set Push Button Event Inturrupt
    Switch_1.rise(&push_event);

    osKernelInitialize(); 
    osThreadCreate(osThread(motor_thread), NULL);
    osThreadCreate(osThread(mag_reading), NULL);
    osThreadCreate(osThread(print_data_thread), NULL);
    osThreadCreate(osThread(inductor_X1_thread), NULL);
    osKernelStart ();

}