#include "cmsis_os.h"
#include "Inductor.h"
#include "ak8963.h"
#include "main.h"
#include "mbed.h"
#include "PID.h"
#include "Servo.h"


int Z_pv;
float z_power = 0;
I2C i2c_1(PB_9, PB_8);
bool mutex;

void motor_thread(void const *argument) {
    while (true) {
           // power_val = pot1.read();
            ;//tunnel_motor = power_val;
//            osDelay(50);
    }
}

void Inductors(void const *argument) {
    //waiting on logic gates
    DigitalOut in2(PA_7);
    in2 = 0;

    //Z Direction Inductor Settings
    Inductor inductor_Z(inductor_Z_enA, inductor_Z_in1);

    inductor_Z.setInputLimits(inductor_Z_input_min, inductor_Z_input_max);
//    inductor_Z.setInputLimits(-60, 160);

    inductor_Z.setOutputLimits(inductor_Z_output_min, inductor_Z_output_max);
//    make this -1 to 1 and change polarity accordingly
//    inductor_Z.setOutputLimits(0.0, 1.0);

    inductor_Z.setTunings(inductor_Z_Kc,inductor_Z_Kd,inductor_Z_Ki);
//    inductor_Z.setTunings(0.001,1,0);

    inductor_Z.setMode(AUTO_MODE);

    inductor_Z.setInterval(.1);

    inductor_Z.setSetPoint(inductor_Z_set_point);


    //temporary
    inductor_Z.setPolarity(1);


    while (true) {
        inductor_Z.setProcessValue(Z_pv);
        z_power = inductor_Z.compute();
        //this should be a 100hz IRQ
        inductor_Z.setPower(z_power);
        osDelay(8);
    }

}

void mag_reading(void const *argument) {

    osDelay(20);
    //Magnetometer declaration
    AK8963 ak8963(&i2c_1, AK8963::SLAVE_ADDR_1);
    osDelay(20);

    pc.printf("Checking AK8963 Connectivity\r\n");
    osDelay(20);
    // Checks connectivity
    if(ak8963.checkConnection() != AK8963::SUCCESS) {
       pc.printf("Failed to communicate with AK8963\r\n");
    }
    else {
    ; //nuthin
    }
    
        pc.printf("Initiating Continuous Read Mode\r\n");
    // Puts the device into continuous measurement mode.
    if(ak8963.setOperationMode(AK8963::MODE_CONTINUOUS_2) != AK8963::SUCCESS) {
        pc.printf("Failed to enter continuous mode\r\n");
    }

    while(true) {
        // checks DRDY
        if (ak8963.isDataReady() == AK8963::DATA_READY) {
            AK8963::MagneticVector mag;
            ak8963.getMagneticVector(&mag);
            Z_pv = mag.mz;
            pc.printf("%f\r\n", mag.mz);

            osDelay(10);
//            pc.printf("R");
//            osDelay(10);
        } else if (ak8963.isDataReady() == AK8963::NOT_DATA_READY) {
            // Nothing to do.
        } else {
            if(ak8963.checkConnection() != AK8963::SUCCESS) {
                pc.printf("Failed to communicate with AK8963\r\n");
                if(ak8963.setOperationMode(AK8963::MODE_CONTINUOUS_2) != AK8963::SUCCESS) {
                    pc.printf("Failure, Check Connection\r\n");
                }
            }
        }
    }
}

int main() {
    timer.attach(&active_blink, .5);
    osThreadCreate(osThread(motor_thread), NULL);
    osThreadCreate(osThread(mag_reading), NULL);
    osThreadCreate(osThread(Inductors), NULL);
    //need to create a com thread

    while (true) {
        osDelay(osWaitForever);
    }
}

void active_blink() {
    led1 = !led1;
}