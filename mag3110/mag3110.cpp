#include "mbed.h"
#include "mag3110.h"

MAG3110::MAG3110(PinName sda, PinName scl): m_i2c(sda, scl), 
    m_addr(0x1d) {
        
    char cmd[2];

    cmd[0] = MAG3110_CTRL_REG2;
    cmd[1] = MAG3110_AUTO_MRST_EN;
    m_i2c.write(m_addr, cmd, 2);
    }

void MAG3110::enable() {
    uint8_t data[2];
    readRegs( MAG3110_CTRL_REG1, &data[1], 1);
    data[1] |= 0x01;
    data[0] = MAG3110_CTRL_REG1;
    writeRegs(data, 2);

}

void MAG3110::disable(void) {
    uint8_t data[2];
    readRegs( MAG3110_CTRL_REG1, &data[1], 1);
    data[1] &= 0xFE;
    data[0] = MAG3110_CTRL_REG1;
    writeRegs(data, 2);
}

void MAG3110::readRegs(int addr, uint8_t * data, int len)
{
    char cmd[1];
    cmd[0] = addr;
    m_i2c.write( m_addr, cmd, 1, true);
    m_i2c.read( m_addr, (char *) data, len);
    return;
}

void MAG3110::writeRegs(uint8_t * data, int len) {
    m_i2c.write(m_addr, (char *)data, len);
}

uint32_t MAG3110::whoAmI() {
    uint8_t who_am_i = 0;
    readRegs(MAG3110_WHO_AM_I, &who_am_i, 1);
    return (uint32_t) who_am_i;
}

uint32_t MAG3110::dataReady(void) {
    uint8_t stat = 0;
    readRegs(MAG3110_DR_STATUS, &stat, 1);
    return (uint32_t) stat;
}

int16_t MAG3110::getMagAxis(uint8_t addr) {
    int16_t acc;
    uint8_t res[2];
    readRegs(addr, res, 2);

    acc = (res[0] << 8) | res[1];

    return acc;
}