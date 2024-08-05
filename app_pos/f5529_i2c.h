#ifndef MSP430F5529_I2C_H_
#define MSP430F5529_I2C_H_
#include "driverlib.h"

// extern void Init_I2C_GPIO(void);
// extern void I2C_init(uint8_t slaveAddress);
void iic_writeByte(uint8_t byte);
void iic_writeWord(uint16_t word);
void iic_readByte(uint8_t RegAddr, uint8_t* b);
void iic_readBytes(uint8_t RegAddr, uint8_t length, uint8_t* data);

#endif /* MSP430F5529_I2C_H_ */
