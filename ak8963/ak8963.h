#ifndef AK8963_H
#define AK8963_H

#include "mbed.h"

/**
 * This is a device driver of AK8963.
 *
 * @note AK8963 is a 3-axis magnetometer (magnetic sensor) device manufactured by AKM.
 * 
 * Example:
 * @code
 * #include "mbed.h"
 * #include "ak8963.h"
 * 
 * #define I2C_SPEED_100KHZ    100000
 * #define I2C_SPEED_400KHZ    400000
 * 
 * int main() {
 *     // Creates an instance of I2C
 *     I2C connection(I2C_SDA0, I2C_SCL0);
 *     connection.frequency(I2C_SPEED_100KHZ);
 *
 *     // Creates an instance of AK8963
 *     AK8963 ak8963(&connection, AK8963::SLAVE_ADDR_1);
 *     
 *     // Checks connectivity
 *     if(ak8963.checkConnection() != AK8963::SUCCESS) {
 *         // Failed to check device connection.
 *         // - error handling here -
 *     }
 *     
 *     // Puts the device into continuous measurement mode.
 *     if(ak8963.setOperationMode(AK8963::MODE_CONTINUOUS_1) != AK8963::SUCCESS) {
 *         // Failed to set the device into continuous measurement mode.
 *         // - error handling here -
 *     }
 *
 *     while(true) {
 *         // checks DRDY
 *         if (statusAK8963 == AK8963::DATA_READY) {
 *             AK8963::MagneticVector mag;
 *             ak8963.getMagneticVector(&mag);
 *             // You may use serial output to see data.
 *             // serial.printf("%d,%5.1f,%5.1f,%5.1f\n",
 *             //    mag.isOverflow,
 *             //    mag.mx, mag.my, mag.mz);
 *             statusAK8963 = AK8963::NOT_DATA_READY;
 *         } else if (statusAK8963 == AK8963::NOT_DATA_READY) {
 *             // Nothing to do.
 *         } else {
 *             // - error handling here -
 *         }
 *     }
 * }
 * @endcode
 */
class AK8963
{
public:
    /** 
     * Possible slave address of AK8963. Selected by CAD1 and CAD0 pins. 
     */
    typedef enum {        
        SLAVE_ADDR_1 = 0x0c,    /**< CAD0 = Low, CAD1 = Low */
        SLAVE_ADDR_2 = 0x0d,    /**< CAD0 = High, CAD1 = Low */
        SLAVE_ADDR_3 = 0x0e,    /**< CAD0 = Low, CAD1 = High */
        SLAVE_ADDR_4 = 0x0f,    /**< CAD0 = High, CAD1 = High */
    } SlaveAddress;
    
    /**
     * Available opration modes in AK8963.
     */
    typedef enum {
        MODE_POWER_DOWN = 0x00,         /**< Power-down mode */
        MODE_SINGLE_MEASUREMENT = 0x01, /**< Single measurement mode */
        MODE_CONTINUOUS_1 = 0x02,       /**< Continuous measurement mode 1, 8 Hz */
        MODE_CONTINUOUS_2 = 0x06,       /**< Continuous measurement mode 2, 100 Hz */
    } OperationMode;

    /**
     * Status of function. 
     */
    typedef enum {
        SUCCESS,               /**< The function processed successfully. */
        ERROR_I2C_READ,        /**< Error related to I2C read. */
        ERROR_I2C_WRITE,       /**< Error related to I2C write. */
        ERROR,                 /**< General Error */
        DATA_READY,            /**< Data ready */
        NOT_DATA_READY,        /**< Data ready is not asserted. */
    } Status;
    
    /**
     * Structure to hold a magnetic vector.
     */
    typedef struct {
        float mx;        /**< x component */
        float my;        /**< y component */
        float mz;        /**< z component */
        bool isOverflow; /**< Indicating magnetic sensor overflow */
    } MagneticVector;

    /**
     * Constructor.
     *
     * @param conn instance of I2C
     * @param addr slave address of the device
     */
    AK8963(I2C *conn, SlaveAddress addr);

    /**
     * Check the connection. 
     *
     * @note Connection check is performed by reading a register which has a fixed value and verify it.
     *
     * @return Returns SUCCESS when succeeded, otherwise returns another code.
     */
    Status checkConnection();

    /**
     * Sets device operation mode.
     *
     * @param mode device opration mode
     *
     * @return Returns SUCCESS when succeeded, otherwise returns another code.
     */
    Status setOperationMode(OperationMode mode);

    /**
     * Check if data is ready, i.e. measurement is finished.
     *
     * @return Returns DATA_READY if data is ready or NOT_DATA_READY if data is not ready. If error happens, returns another code.
     */
    Status isDataReady();

    /**
     * Gets magnetic vector from the device.
     *
     * @param vec Pointer to a instance of MagneticVector
     *
     * @return Returns SUCCESS when succeeded, otherwise returns another code.
     */
    Status getMagneticVector(MagneticVector *vec);

private:
    /**
     * Sensitivity adjustment data.
     */
    typedef struct {
        unsigned char x;   /**< sensitivity adjustment value for x */
        unsigned char y;   /**< sensitivity adjustment value for y */
        unsigned char z;   /**< sensitivity adjustment value for z */
    } SensitivityAdjustment;
    
    /**
     * Holds sensitivity adjustment values
     */
    SensitivityAdjustment sensitivityAdjustment;
    
    /**
     * Holds a pointer to an I2C object. 
     */
    I2C *connection;
    
    /**
     * Holds the slave address of AK8963. 
     */
    SlaveAddress slaveAddress;
    
    /** 
     * Writes data to the device. 
     * @param registerAddress register address to be written
     * @param data data to be written
     * @param length of the data
     * @return Returns SUCCESS when succeeded, otherwise returns another.
     */
    Status write(char registerAddress, const char *data, int length);

    /** 
     *  Reads data from device. 
     * @param registerAddress register address to be read
     * @param buf buffer to store the read data
     * @param length bytes to be read
     * @return Returns SUCCESS when succeeded, otherwise returns another.
     */
    Status read(char registerAddress, char *buf, int length);
    
    /**
     * Gets magnetic data, from the register ST1 (0x02) to ST2 (0x09), from the device.
     * 
     * @param buf buffer to store the data read from the device.
     * 
     * @return Returns SUCCESS when succeeded, otherwise returns another.
     */
    Status getData(char *buf);
    
    /**
     * Gets sensitivity adjustment values from the register ASAX, ASAY, and ASAZ.
     *
     * @param buf buffer to store sensitivity adjustment data
     *
     * @return SUCCESS when succeeded, otherwise returns another.
     */
    Status getSensitivityAdjustment(SensitivityAdjustment *sns);

};

#endif
