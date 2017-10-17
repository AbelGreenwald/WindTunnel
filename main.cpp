#include "cmsis_os.h"
#include "Inductor.h"
#include "ak8963.h"
#include "main.h"
#include "mbed.h"
#include "PID.h"
#include "Servo.h"

volatile unsigned long num_ms = 0;
volatile float z_power, Z_pv;
I2C i2c_1(PB_9, PB_8);

osMutexId mag_1_Sem;
osMutexDef(mag_1_Sem);


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
    in2 = 1;

    //Z Direction Inductor Settings
    Inductor inductor_Z(inductor_Z_enA, inductor_Z_in1);
    
    inductor_Z.setInputLimits(inductor_Z_input_min, inductor_Z_input_max);
    inductor_Z.setOutputLimits(inductor_Z_output_min, inductor_Z_output_max);
    inductor_Z.setTunings(inductor_Z_Kc,inductor_Z_Ki,inductor_Z_Kd);
    inductor_Z.setMode(AUTO_MODE);
    inductor_Z.setInterval(inductor_Z_timestep);
    inductor_Z.setSetPoint(inductor_Z_set_point);

    //temporary
    inductor_Z.setPolarity(0);

    while (true) {
        osMutexWait(mag_1_Sem, 0);
        inductor_Z.setProcessValue(Z_pv);
        z_power = inductor_Z.compute();
        pc.printf("%f\t%f\r\n", z_power, Z_pv );
       
 //       inductor_Z.setPower(z_power);
        if (z_power <= 0.0) {
            //normal polarity
            z_power = -(z_power);
            in2 = 0;
            inductor_Z.setPolarity(1);
            inductor_Z.setPower(z_power);
//            pc.printf("negative\r\n");
     
        }
        if (z_power > 0.0) {
            //flip polarity
            in2 = 1;
            inductor_Z.setPolarity(0);
            inductor_Z.setPower(z_power);
//            pc.printf("positive\r\n");
        }
        osDelay(inductor_Z_timestep);
        osMutexRelease(mag_1_Sem);
    }

}

void mag_reading(void const *argument) {

    osDelay(20);
    //Magnetometer declaration
    AK8963 ak8963(&i2c_1, AK8963::SLAVE_ADDR_1);
    osDelay(20);
//    pc.printf("Checking AK8963 Connectivity\r\n");
    osDelay(20);
    // Checks connectivity
    if(ak8963.checkConnection() != AK8963::SUCCESS) {
       pc.printf("Failed to communicate with AK8963\r\n");
    }
    else {
    ; //nuthin
    }
    
//        pc.printf("Initiating Continuous Read Mode\r\n");
    // Puts the device into continuous measurement mode.
    if(ak8963.setOperationMode(AK8963::MODE_CONTINUOUS_2) != AK8963::SUCCESS) {
        pc.printf("Failed to enter continuous mode\r\n");
    }

    while(true) {
        // checks DRDY
        if (ak8963.isDataReady() == AK8963::DATA_READY) {
            osMutexWait(mag_1_Sem, 10);
            AK8963::MagneticVector mag;
            ak8963.getMagneticVector(&mag);
            Z_pv = mag.mz;
            osMutexRelease(mag_1_Sem);
            //TODO: if mag overflow, hold at max
//            pc.printf("val=%i\r\n", val);
//            osDelay(10);
        } else if (ak8963.isDataReady() == AK8963::NOT_DATA_READY) {
//            pc.printf("NR\r\n");
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
    osKernelStart();
    timer.attach(&active_blink, .5);
    ms.attach_us(&count_ms, 10);
    osMutexCreate(osMutex(mag_1_Sem));
    osThreadCreate(osThread(motor_thread), NULL);
    osThreadCreate(osThread(Inductors), NULL);
    osThreadCreate(osThread(mag_reading), NULL);
    //need to create a com thread

    while (true) {
        osDelay(osWaitForever);
    }
}

void active_blink() {
    led1 = !led1;
}
void count_ms() {
    num_ms = num_ms + 1;
}
