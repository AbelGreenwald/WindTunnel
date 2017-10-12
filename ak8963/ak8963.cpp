#include "ak8963.h"

// REGISTER MAP
#define AK8963_REG_WIA             0x00
#define AK8963_REG_INFO            0x01
#define AK8963_REG_ST1             0x02
#define AK8963_REG_HXL             0x03
#define AK8963_REG_HXH             0x04
#define AK8963_REG_HYL             0x05
#define AK8963_REG_HYH             0x06
#define AK8963_REG_HZL             0x07
#define AK8963_REG_HZH             0x08
#define AK8963_REG_ST2             0x09
#define AK8963_REG_CNTL1           0x0A
#define AK8963_REG_CNTL2           0x0B
#define AK8963_REG_ASTC            0x0C
#define AK8963_REG_TS1             0x0D
#define AK8963_REG_TS2             0x0E
#define AK8963_REG_I2CDIS          0x0F
#define AK8963_REG_ASAX            0x10
#define AK8963_REG_ASAY            0x11
#define AK8963_REG_ASAZ            0x12
#define AK8963_REG_RSV             0x13

// BITS in CNTL1 register
#define AK8963_CNTL1_MODE_PDN       0x00     // POWER DOWN MODE
#define AK8963_CNTL1_MODE_SNG       0x01     // SINGLE MEASUREMENT MODE
#define AK8963_CNTL1_MODE_CNT1      0x02     // CONTINUOUS MEASUREMENT MODE 1
#define AK8963_CNTL1_MODE_CNT2      0x06     // CONTINUOUS MEASUREMENT MODE 2
#define AK8963_CNTL1_MODE_EXT       0x04     // EXTERNAL TRIGGER MEASUREMENT MODE
#define AK8963_CNTL1_MODE_TEST      0x08     // SELF-TEST MODE
#define AK8963_CNTL1_MODE_FUSE_ROM  0x0F     // FUSE ROM ACCESS MODE
#define AK8963_CNTL1_OUTPUT_14BIT   0x00
#define AK8963_CNTL1_OUTPUT_16BIT   0x10

#define AK8963_BIT_MASK_DRDY        0x01

#define AK8963_BIT_MASK_HOFL        0x08

#define AK8963_REG_WIA_VAL          0x48

#define AK8963_BUF_LENGTH_ASA          3
#define AK8963_BUF_LENGTH_BDATA        8
#define AK8963_WAIT_MODE_TRANSITION  100
#define AK8963_DEFAULT_OUTPUT_BIT   AK8963_CNTL1_OUTPUT_16BIT
#define AK8963_SENSITIVITY          (0.15)   // uT/LSB

#define AK8963_WAIT_POWER_DOWN_US    100

#define LEN_ONE_BYTE  1

AK8963::AK8963(I2C *conn, SlaveAddress addr) {
    slaveAddress = addr;
    connection = conn;
    getSensitivityAdjustment(&sensitivityAdjustment);
}

AK8963::Status AK8963::checkConnection() {
    AK8963::Status status = AK8963::SUCCESS;
    
    // Gets the WIA register value.
    char wiaValue = 0;
    if ((status=AK8963::read(AK8963_REG_WIA, &wiaValue, LEN_ONE_BYTE)) != SUCCESS) {
        return status;
    }

    // Checks the obtained value equals to the supposed value.
    if (wiaValue != AK8963_REG_WIA_VAL) {
        return AK8963::ERROR;
    }
    
    return status;
}

AK8963::Status AK8963::read(char registerAddress, char *buf, int length) {
    // Writes a start address
    if (connection->write((slaveAddress << 1), &registerAddress, LEN_ONE_BYTE) != 0) {
        // I2C write failed.
        return AK8963::ERROR_I2C_WRITE;
    }
    
    // Reads register data
    if (connection->read((slaveAddress << 1), buf, length) != 0) {
        // I2C read failed.
        return AK8963::ERROR_I2C_READ;
    }
    
    return AK8963::SUCCESS;
}

AK8963::Status AK8963::write(char registerAddress, const char *buf, int length) {
    int bufLength = length + 1;
    char data[bufLength];

    // Creates data to be sent.
    data[0] = registerAddress;    
    for (int i=0; i < length; i++) {
        data[1+i] = buf[i];
    }
    
    // Writes data
    if (connection->write((slaveAddress << 1), data, bufLength) != 0) {
        // I2C write failed.
        return AK8963::ERROR_I2C_WRITE;
    }

    return AK8963::SUCCESS;
}

AK8963::Status AK8963::isDataReady() {
    AK8963::Status status = AK8963::ERROR;
    
    // Gets the ST1 register value.
    char st1Value = 0;
    if ((status=AK8963::read(AK8963_REG_ST1, &st1Value, LEN_ONE_BYTE)) != AK8963::SUCCESS) {
        // I2C read failed.
        return status;
    }

    // Sets a return status corresponds to the obtained value.    
    if ((st1Value & AK8963_BIT_MASK_DRDY) > 0) {
        status = AK8963::DATA_READY;
    } else {
        status = AK8963::NOT_DATA_READY;
    }
    
    return status;
}

AK8963::Status AK8963::getData(char *buf) {
    AK8963::Status status = AK8963::ERROR;
    
    if ((status=AK8963::read(AK8963_REG_ST1, buf, AK8963_BUF_LENGTH_BDATA)) != AK8963::SUCCESS) {
        // I2C read failed.
        return status;
    }
    
    return status;
}

AK8963::Status AK8963::setOperationMode(AK8963::OperationMode mode) {
    AK8963::Status status = AK8963::ERROR;
    
    // The device has to be put into power-down mode first before switching mode.
    char buf = AK8963::MODE_POWER_DOWN;
    if (mode != AK8963::MODE_POWER_DOWN) {
        if ((status=AK8963::write(AK8963_REG_CNTL1, &buf, LEN_ONE_BYTE)) != AK8963::SUCCESS) {
            // I2C write failed.
            return status;
        }
        wait_us(AK8963_WAIT_POWER_DOWN_US);
    }


    // Switch to the specified mode.
    buf = (mode | AK8963_CNTL1_OUTPUT_16BIT);    
    if ((status=AK8963::write(AK8963_REG_CNTL1, &buf, LEN_ONE_BYTE)) != AK8963::SUCCESS) {
        // I2C write failed.
        return status;
    }
    
    wait_us(AK8963_WAIT_POWER_DOWN_US);
    
    return AK8963::SUCCESS;
}

AK8963::Status AK8963::getMagneticVector(MagneticVector *vec) {
    AK8963::Status status = AK8963::ERROR;
    char buf[AK8963_BUF_LENGTH_BDATA];
    
    if ((status=AK8963::getData(buf)) != AK8963::SUCCESS) {
        // Failed to get data.
        return status;
    }
    
    // Checks sensor overflow
    if ((buf[7] & AK8963_BIT_MASK_HOFL) > 0) {
        // Magnetic sensor overflow
        vec->isOverflow = true;
    } else {
        // Magnetic sensor didn't overflow.
        vec->isOverflow = false;
        // Calculates magnetic field value
        int16_t xi = (int16_t)((buf[2] << 8) | buf[1]);
        int16_t yi = (int16_t)((buf[4] << 8) | buf[3]);
        int16_t zi = (int16_t)((buf[6] << 8) | buf[5]);
        vec->mx = (float)(xi * ((sensitivityAdjustment.x + 128)/256.0) * AK8963_SENSITIVITY);
        vec->my = (float)(yi * ((sensitivityAdjustment.y + 128)/256.0) * AK8963_SENSITIVITY);
        vec->mz = (float)(zi * ((sensitivityAdjustment.z + 128)/256.0) * AK8963_SENSITIVITY);
    }
        
    return AK8963::SUCCESS;
}


AK8963::Status AK8963::getSensitivityAdjustment(SensitivityAdjustment *sns) {
    AK8963::Status status = AK8963::ERROR;
    
    // Set the device into the fuse ROM access mode.
    char data = AK8963_CNTL1_MODE_FUSE_ROM;
    if ((status=AK8963::write(AK8963_REG_CNTL1, &data, LEN_ONE_BYTE)) != AK8963::SUCCESS) {
        // I2C write failed.
        return status;
    }
    
    // Wait
    wait_us(AK8963_WAIT_POWER_DOWN_US);
    
    // Read the fuse ROM
    char buf[AK8963_BUF_LENGTH_ASA];
    if ((status=AK8963::read(AK8963_REG_ASAX, buf, AK8963_BUF_LENGTH_ASA)) != AK8963::SUCCESS) {
        // I2C read failed.
        return status;
    }
    
    sns->x = buf[0];
    sns->y = buf[1];
    sns->z = buf[2];
    
    // Set the device into the power-down mode
    data = AK8963_CNTL1_MODE_PDN;
    if ((status=AK8963::write(AK8963_REG_CNTL1, &data, LEN_ONE_BYTE)) != AK8963::SUCCESS) {
        // I2C write failed.
        return status;
    }
    
    // Wait
    wait_us(AK8963_WAIT_POWER_DOWN_US);
    
    return AK8963::SUCCESS;  
}

