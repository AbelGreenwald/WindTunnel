
/* Define Pins */
#define inductor_X_l_in1  D9
#define inductor_X_l_in2  D9
#define inductor_X_l_enA  D9

#define inductor_Y_l_in1  D9
#define inductor_Y_l_in2  D9
#define inductor_Y_l_enA  D9

#define inductor_X_h_in1  D9
#define inductor_X_h_in2  D9
#define inductor_X_h_enA  D9

#define inductor_Y_h_in1  D9
#define inductor_Y_h_in2  D9
#define inductor_Y_h_enA  D9

#define inductor_Z_in1    D9
#define inductor_Z_in2    D9
#define inductor_Z_enA    D9

#define inductor_MZ_l_in1 D9
#define inductor_MZ_l_in2 D9
#define inductor_MZ_l_enA D9

#define inductor_MZ_h_in1 D9
#define inductor_MZ_h_in2 D9
#define inductor_MZ_h_enA D9

float mag_val, power_val;
volatile int val1, val2, val3;

/* Set Pins */

// blinking LED
DigitalOut led1(LED1);
//PwmOut for inductor_X1
PwmOut     inductor_X1_enA(D9);
//Switch 1 for inductor_X1
DigitalOut inductor_X1_in1(D8);
//Switch 2 for inductor_X1
DigitalOut inductor_X1_in2(D7);
//Temp Sensor for inductor_X1
AnalogIn inductor_X1_tmp(A4);
// Tunnel Motor ESC Driver
Servo tunnel_motor(D3);
// Timer inturrupt for power LED
Ticker timer;
// inductor_X1 PID
PID inductor_X1_pid(.0015,4,2,.0125);
// delta timer for calculations
Timer timeTimer;
// state output and debug info
Serial pc(USBTX, USBRX, 115200);

/* Function Prototypes  */

void blink(void);
void motor_thread(void const*);
void inductor_X1_thread(void const*);
void mag_reading(void const*);