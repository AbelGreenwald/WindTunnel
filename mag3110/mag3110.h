#include "mbed.h"
/* Author Abel Greenwald
Important specs (FROM NXP mag3110 datasheet)
Supply voltage VDD -0.3 to +3.6 V
Input voltage on any control pin (SCL, SDA) Vin -0.3 to VDDIO + 0.3V
Maximum applied magnetic/field B(max) 100,000 μT
Operating temperature range -40 to +85 °C
Storage temperature range -40 to +125 °C
see http://www.nxp.com/assets/documents/data/en/data-sheets/MAG3110.pdf
*/

/* MAG3110 I2C Address */
#define MAG3110_I2C_ADDRESS 0x0E

/* MAG3110 Magnetometer Registers */
#define MAG3110_DR_STATUS    0x00 //Data ready status per axis
#define MAG3110_OUT_X_MSB    0x01 //Bits [15:8] of X measurement
#define MAG3110_OUT_X_LSB    0x02 //Bits [7:0] of X measurement
#define MAG3110_OUT_Y_MSB    0x03 //Bits [15:8] of Y measurement
#define MAG3110_OUT_Y_LSB    0x04 //Bits [7:0] of Y measurement
#define MAG3110_OUT_Z_MSB    0x05 //Bits [15:8] of Z measurement
#define MAG3110_OUT_Z_LSB    0x06 //Bits [7:0] of Z measurement
#define MAG3110_WHO_AM_I     0x07 //Device ID Number
#define MAG3110_SYSMOD       0x08 //Current System Mode
#define MAG3110_OFF_X_MSB    0x09 //Bits [14:7] of user X offset
#define MAG3110_OFF_X_LSB    0x0A //Bits [6:0] of user X offset
#define MAG3110_OFF_Y_MSB    0x0B //Bits [14:7] of user Y offset
#define MAG3110_OFF_Y_LSB    0x0C //Bits [6:0] of user Y offset
#define MAG3110_OFF_Z_MSB    0x0D //Bits [14:7] of user Z offset
#define MAG3110_OFF_Z_LSB    0x0E //Bits [6:0] of user Z offset
#define MAG3110_DIE_TEMP     0x0F //Temperature, signed 8 bits in C
#define MAG3110_CTRL_REG1    0x10 //Operation modes
#define MAG3110_CTRL_REG2    0x11 //Operation modes


/* MAG3110 WHO_AM_I Response */
#define MAG3110_WHO_AM_I_RSP        0xC4 // (factory programmed)

/////////////////////////////////////////
// MAG3110 Commands and Settings       //
/////////////////////////////////////////

//CTRL_REG1 Settings
//Output Data Rate/Oversample Settings
//DR_OS_80_16 -> Output Data Rate = 80Hz, Oversampling Ratio = 16
//Bit 7   Bit 6   Bit 5   Bit 4   Bit 3   Bit 2    Bit 1    Bit 0
//DR2     DR1     DR0     OS1     OS 0    FR       TM       AC
#define MAG3110_DR_OS_80_16         0x00
#define MAG3110_DR_OS_40_32         0x08
#define MAG3110_DR_OS_20_64         0x10
#define MAG3110_DR_OS_10_128        0x18
#define MAG3110_DR_OS_40_16         0x20
#define MAG3110_DR_OS_20_32         0x28
#define MAG3110_DR_OS_10_64         0x30
#define MAG3110_DR_OS_5_128         0x38
#define MAG3110_DR_OS_20_16         0x40
#define MAG3110_DR_OS_10_32         0x48
#define MAG3110_DR_OS_5_64          0x50
#define MAG3110_DR_OS_2_5_128       0x58
#define MAG3110_DR_OS_10_16         0x60
#define MAG3110_DR_OS_5_32          0x68
#define MAG3110_DR_OS_2_5_64        0x70
#define MAG3110_DR_OS_1_25_128      0x78
#define MAG3110_DR_OS_5_16          0x80
#define MAG3110_DR_OS_2_5_32        0x88
#define MAG3110_DR_OS_1_25_64       0x90
#define MAG3110_DR_OS_0_63_128      0x98
#define MAG3110_DR_OS_2_5_16        0xA0
#define MAG3110_DR_OS_1_25_32       0xA8
#define MAG3110_DR_OS_0_63_64       0xB0
#define MAG3110_DR_OS_0_31_128      0xB8
#define MAG3110_DR_OS_1_25_16       0xC0
#define MAG3110_DR_OS_0_63_32       0xC8
#define MAG3110_DR_OS_0_31_64       0xD0
#define MAG3110_DR_OS_0_16_128      0xD8
#define MAG3110_DR_OS_0_63_16       0xE0
#define MAG3110_DR_OS_0_31_32       0xE8
#define MAG3110_DR_OS_0_16_64       0xF0
#define MAG3110_DR_OS_0_08_128      0xF8

//Other CTRL_REG1 Settings
#define MAG3110_FAST_READ           0x04
#define MAG3110_TRIGGER_MEASUREMENT 0x02
#define MAG3110_ACTIVE_MODE         0x01
#define MAG3110_STANDBY_MODE        0x00

//CTRL_REG2 Settings
#define MAG3110_AUTO_MRST_EN        0x80
#define MAG3110_RAW_MODE            0x20
#define MAG3110_NORMAL_MODE         0x00
#define MAG3110_MAG_RST             0x10

//SYSMOD Readings
#define MAG3110_SYSMOD_STANDBY      0x00
#define MAG3110_SYSMOD_ACTIVE_RAW   0x01
#define MAG3110_SYSMOD_ACTIVE       0x02

#define MAG3110_X_AXIS 1
#define MAG3110_Y_AXIS 3
#define MAG3110_Z_AXIS 5

class MAG3110
{
 public:
  MAG3110(PinName sda,PinName scl);
  void enable(void);
  void disable(void);
  uint32_t whoAmI(void);
  uint32_t dataReady(void);
  //Public methods
  int16_t getMagAxis(uint8_t addr);
 private:
 //Private variables
  I2C m_i2c;
  char m_addr;
  void readRegs(int addr, uint8_t * data, int len);
  void writeRegs(uint8_t * data, int len);
};