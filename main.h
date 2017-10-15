#ifndef MAIN_H
#define MAIN_H

/* Define Pins */
#define inductor_X_l_in1  NaN
#define inductor_X_l_in2  NaN
#define inductor_X_l_enA  NaN

#define inductor_Y_l_in1  NaN
#define inductor_Y_l_in2  NaN
#define inductor_Y_l_enA  NaN

#define inductor_X_h_in1  NaN
#define inductor_X_h_in2  NaN
#define inductor_X_h_enA  NaN

#define inductor_Y_h_in1  NaN
#define inductor_Y_h_in2  NaN
#define inductor_Y_h_enA  NaN

#define inductor_Z_in1    PB_6
#define inductor_Z_in2    PA_7
#define inductor_Z_enA    PC_9

/* Define Calibration Settings */
#define inductor_Z_input_min 340
#define inductor_Z_input_max 2500
#define inductor_Z_set_point 500
#define inductor_Z_output_min 1.0
#define inductor_Z_output_max -1.0
#define inductor_Z_timestep .01
//should come from rlocus
#define inductor_Z_Kc 1.0
#define inductor_Z_Kd 0.0
#define inductor_Z_Ki 0.0


#define inductor_MZ_l_in1 NaN
#define inductor_MZ_l_in2 NaN
#define inductor_MZ_l_enA NaN

#define inductor_MZ_h_in1 NaN
#define inductor_MZ_h_in2 NaN
#define inductor_MZ_h_enA NaN



float mag_val, power_val;
volatile int val1, val2, val3;


/* Set Pins */

// blinking LED
DigitalOut led1(LED1);

//Temp Sensor for inductor_X1
//AnalogIn inductor_X1_tmp(A4);
// Tunnel Motor ESC Driver
//Servo tunnel_motor(D3);
// Timer inturrupt for power LED
Ticker timer;
// state output and debug info
Serial pc(USBTX, USBRX, 115200);
//I2c Channel 1 definition


/* Function Prototypes  */

void active_blink(void);
void motor_thread(void const*);
void Inductors(void const*);
void mag_reading(void const*);

/* Threads  */
osThreadDef(motor_thread, osPriorityLow, 1024);
osThreadDef(mag_reading, osPriorityNormal, DEFAULT_STACK_SIZE);
osThreadDef(Inductors, osPriorityNormal, 4096); //4K stack

#endif