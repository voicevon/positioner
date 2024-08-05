

// MSP430F5529 DriverLib 库函数I2C驱动OLED屏幕
// https://blog.csdn.net/weixin_44457994/article/details/119398194


// USCI_B_I2C 函数库介绍
// https://blog.csdn.net/weixin_62179882/article/details/131743401

// P3.0为SDA，P3.1为SCL

#include "driverlib.h"
#include "usci_b_i2c.h"
#include <cstdio>

#define I2C_USCI_BASE USCI_B0_BASE
#define I2C_USCI_VECTOR USCI_B0_VECTOR
#define I2C_USCI_IV UCB0IV

#define I2C_BUF_LENGTH 32
char i2c_buf[I2C_BUF_LENGTH];    // Buffer to send to slave address.  不包含第一个字节：地址.
static uint8_t i2c_buf_len = 6;
static uint8_t i2c_buf_cur = 0;

static uint8_t *i2c_rx_buf = 0;
static uint8_t i2c_rx_buf_len = 0;

void iic_scan_devices(){
    for (int slave_addr = 1; slave_addr<127; slave_addr++){
        USCI_B_I2C_setSlaveAddress(I2C_USCI_BASE, slave_addr);
        bool x = USCI_B_I2C_masterSendSingleByteWithTimeout(I2C_USCI_BASE, 0x00, 9999);
        if(x) {
            printf("Got response, can set a break point here.");
        }
    }

}

/***************************************************************************//**
 * @brief  设置 I2C 使用的GPIO,  时钟源，分频
 * @param  none
 * @return none
 ******************************************************************************/

void I2C_init(uint8_t slaveAddress)
{
    // P3.0 as SCLK?  P3.1 as SDA?
    /* Select I2C function for I2C_SDA & I2C_SCL */
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P3,
            GPIO_PIN0 | GPIO_PIN1);

  /* I2C Master Configuration Parameter */
  USCI_B_I2C_initMasterParam i2cConfig =
  {
    USCI_B_I2C_CLOCKSOURCE_SMCLK,
    UCS_getSMCLK(),
    USCI_B_I2C_SET_DATA_RATE_400KBPS
  };
  /* Initialize USCI_B0 and I2C Master to communicate with slave devices*/
  USCI_B_I2C_initMaster(I2C_USCI_BASE, &i2cConfig);

  /* Enable I2C Module to start operations */
  USCI_B_I2C_setMode(I2C_USCI_BASE, USCI_B_I2C_TRANSMIT_MODE);
  USCI_B_I2C_enable(I2C_USCI_BASE);

  // Specify slave address
  USCI_B_I2C_setSlaveAddress(I2C_USCI_BASE, slaveAddress);
//   USCI_B_I2C_clearInterrupt(I2C_USCI_BASE, USCI_B_I2C_RECEIVE_INTERRUPT);
//   USCI_B_I2C_enableInterrupt(I2C_USCI_BASE, USCI_B_I2C_RECEIVE_INTERRUPT);
  USCI_B_I2C_clearInterrupt(I2C_USCI_BASE, USCI_B_I2C_TRANSMIT_INTERRUPT);
  USCI_B_I2C_enableInterrupt(I2C_USCI_BASE, USCI_B_I2C_TRANSMIT_INTERRUPT);

    // command for digital
    // i2c_buf[0] = 0xc8;
    i2c_buf[0] = 0xcc;
    i2c_buf[1] = 0x33;

    // digital data
    i2c_buf[2] = 0xff;
    i2c_buf[3] = 0x00;
    i2c_buf[4] = 0xff;
    i2c_buf[5] = 0x00;

}


// write a byte to specific register, cannot called in interrupt context 
// 发送一个字节， i2c_buf[0], 然后开始睡眠， 
// 当该字节发送完毕时，中断服务会接着发送其他字节,  i2c_buf[1] .. i2c_buf[i2c_buf_len]。
// 注意： 这个字节是有 Start / ReStart 标志的。
void iic_writeByte(uint8_t byte)
{
    while (USCI_B_I2C_isBusBusy(I2C_USCI_BASE));

    USCI_B_I2C_setMode(I2C_USCI_BASE, USCI_B_I2C_TRANSMIT_MODE);

    // Initiate start and send first character
    i2c_buf[0] = byte;
    i2c_buf_cur = 1;
    i2c_buf_len = 6;
    USCI_B_I2C_masterSendMultiByteStart(I2C_USCI_BASE, i2c_buf[0]);
    // USCI_B_I2C_masterSendMultiByteNext(I2C_USCI_BASE, i2c_buf[0]);    // This does not work!

    // wait for end
    __bis_SR_register(GIE + LPM0_bits);
    __no_operation();
}

/* write a word to specific register, cannot called in interrupt context */
void iic_writeWord(uint16_t word)
{
    while (USCI_B_I2C_isBusBusy(I2C_USCI_BASE));

    USCI_B_I2C_setMode(I2C_USCI_BASE, USCI_B_I2C_TRANSMIT_MODE);

    // Initiate start and send first character
    i2c_buf[0] = word >> 8;
    i2c_buf[1] = (uint8_t)word;
    i2c_buf_cur = 1;
    i2c_buf_len = 2;
    USCI_B_I2C_masterSendMultiByteStart(I2C_USCI_BASE, i2c_buf[0]);
    // USCI_B_I2C_masterSendMultiByteNext(uint16_t baseAddress, uint8_t txData)

    // wait for end
    __bis_SR_register(GIE + LPM0_bits);
    __no_operation();
}

///* set/clear some bit to specific register, cannot called in interrupt context */
//void writeBit(uint8_t regAddr, uint8_t bitNum, uint8_t data)
//{
//    uint8_t b = 0;
//    readByte(regAddr, &b);
//    delay_ms(2);
//    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
//    writeByte(regAddr, b);
//}

/* read some byte from specific register, cannot called in interrupt context */
void iic_readByte(uint8_t RegAddr, uint8_t* b)
{
	while (USCI_B_I2C_isBusBusy(I2C_USCI_BASE));

    // send address
    USCI_B_I2C_setMode(I2C_USCI_BASE, USCI_B_I2C_TRANSMIT_MODE);
    i2c_buf_cur = 1;
    i2c_buf_len = 1;
    USCI_B_I2C_masterSendSingleByte(I2C_USCI_BASE, RegAddr);

    // receive
    USCI_B_I2C_setMode(I2C_USCI_BASE, USCI_B_I2C_RECEIVE_MODE);
    i2c_rx_buf = b;
    i2c_rx_buf_len = 1;
    USCI_B_I2C_masterReceiveSingleStart(I2C_USCI_BASE);

//     wait for end
    __bis_SR_register(GIE + LPM0_bits);
    __no_operation();
}

void iic_readBytes(uint8_t RegAddr, uint8_t length, uint8_t* data)
{
	while (USCI_B_I2C_isBusBusy(I2C_USCI_BASE));
    // send address
    USCI_B_I2C_setMode(I2C_USCI_BASE, USCI_B_I2C_TRANSMIT_MODE);
    i2c_buf_cur = 1;
    i2c_buf_len = 1;
    USCI_B_I2C_masterSendSingleByte(I2C_USCI_BASE, RegAddr);

    // receive
    USCI_B_I2C_setMode(I2C_USCI_BASE, USCI_B_I2C_RECEIVE_MODE);
    i2c_rx_buf = data;
    i2c_rx_buf_len = length;
    USCI_B_I2C_masterReceiveMultiByteStart(I2C_USCI_BASE);

    // wait for end
    __bis_SR_register(GIE + LPM0_bits);
    __no_operation();
}

#pragma vector = I2C_USCI_VECTOR
__interrupt void USCI_B0_ISR(void)
{
    switch (__even_in_range(I2C_USCI_IV, 12))
    {
        case USCI_I2C_UCTXIFG:
            // I2C 发送中断标志。
            if (i2c_buf_cur < i2c_buf_len)
            {
                // 发送下一个字节。
                USCI_B_I2C_masterSendMultiByteNext( I2C_USCI_BASE, i2c_buf[i2c_buf_cur]);
                i2c_buf_cur++;
            }
            else
            {
                // 所有数据已经发送完毕， 现在向总线发送停止。
                USCI_B_I2C_masterSendMultiByteStop(I2C_USCI_BASE);
                //Clear master interrupt status
                USCI_B_I2C_clearInterrupt(I2C_USCI_BASE,
                                          USCI_B_I2C_TRANSMIT_INTERRUPT);
                __bic_SR_register_on_exit(LPM0_bits);
            }
            break;
        case USCI_I2C_UCRXIFG:
            i2c_rx_buf_len--;
            if(i2c_rx_buf_len)
            {
                if(i2c_rx_buf_len== 1)
                {
                    //Initiate end of reception -> Receive byte with NAK
                    *i2c_rx_buf++ = USCI_B_I2C_masterReceiveMultiByteFinish( I2C_USCI_BASE);
                }
                else
                {
                    //Keep receiving one byte at a time
                    *i2c_rx_buf++ = USCI_B_I2C_masterReceiveMultiByteNext( I2C_USCI_BASE);
                }
            }
            else
            {
                //Receive last byte
                *i2c_rx_buf= USCI_B_I2C_masterReceiveMultiByteNext(I2C_USCI_BASE);
                __bic_SR_register_on_exit(LPM0_bits);
            }
            break;
    }
}
