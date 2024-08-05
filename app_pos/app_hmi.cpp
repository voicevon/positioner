#include "app_hmi.h"
#include "driverlib.h"
#include "intrinsics.h"
#include "machine/_types.h"
#include "msp430f5529.h"
#include "msp430f5xx_6xxgeneric.h"
#include "usci_b_i2c.h"

#include "f5529_i2c.h"

extern void I2C_init(uint8_t slaveAddress);
extern void iic_writeByte(uint8_t byte);

// void I2C_DIO_init(void)
// {
//     // 设置 GPIO　第二功能 P4.1 SDA,   P4.2 SCL
//     #define I2C_SDA_pin 1
//     #define I2C_SCL_pin 2
//     GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P4, (I2C_SDA_pin | I2C_SCL_pin));
// }

// void init_USCI_I2C(void)
// {
//     USCI_B_I2C_initMasterParam I2C_param = {0};

//     I2C_DIO_init();

//     I2C_param.selectClockSource = USCI_B_I2C_CLOCKSOURCE_SMCLK;
//     I2C_param.i2cClk = UCS_getSMCLK();
//     I2C_param.dataRate = USCI_B_I2C_SET_DATA_RATE_100KBPS;

//     USCI_B_I2C_initMaster(USCI_B1_BASE, &I2C_param);

//     USCI_B_I2C_enable(USCI_B1_BASE);
// }


void Hmi::pcf_write_byte(unsigned char address, unsigned char value){
    // 第一部分： PCF地址
    // 第二部分： command, 1个到多个字节
    // 第三部分： display Data, 0个到多个字节。



    // 第一个字节： PCF 地址 + /WRITE
    // 
    USCI_B_I2C_setSlaveAddress(USCI_B1_BASE, 0x70);
    USCI_B_I2C_setMode(USCI_B1_BASE, USCI_B_I2C_TRANSMIT_MODE);

    // 第二部分： pcf command + display data. 数据手册 Page 29.
    // command is one or more bytes,  数据手册 Page 5.
    // 第一字节 Mode_Set ： 0xc8  or 0xcc
    //                      1100 1?00 :  bit3=enabled.   bit2=1/3   bit1_0 =BP0,BP1,BP2,BP3
    // 第二字节 Load_data_pointer: 0x33
    //                      0011 0011:  
    //        digits address begin:   ==33 ==0x21 == 0b 0011 0011
    //        digits address end:     ==40 ==0x28
    // 
    
    unsigned char commands[2] = {0xc8, 0x33};
    USCI_B_I2C_masterSendMultiByteNext(USCI_B1_BASE, commands[0]);
    USCI_B_I2C_masterSendMultiByteNext(USCI_B1_BASE, commands[1]);

    // 第三部分： display data
    // 共4个字节，描述4个 digits.
    unsigned char digits_segment[4] = {0xff,0xff,0xff,0xff};
    USCI_B_I2C_masterSendMultiByteNext(USCI_B1_BASE, digits_segment[0]);
    USCI_B_I2C_masterSendMultiByteNext(USCI_B1_BASE, digits_segment[1]);
    USCI_B_I2C_masterSendMultiByteNext(USCI_B1_BASE, digits_segment[2]);
    USCI_B_I2C_masterSendMultiByteFinish(USCI_B1_BASE, digits_segment[3]);




    // USCI_B_I2C_masterSendSingleByte(USCI_B1_BASE, COMMAND);   

    while(! USCI_B_I2C_masterIsStartSent(USCI_B1_BASE)){

    };
    // USCI_B_I2C_masterSendMultiByteStart(USCI_B1_BASE, uint8_t txData);



}

void Hmi::setup_pcf85176(){
    // I2C_init(56);  // 0x38  == 0b0011,1000
    I2C_init(0x70 >> 1);  // 0x38  == 0b0011,1000


    iic_writeByte(0xc8); 
    // init_USCI_I2C();
    // // I2C address of PCF85176 :  0x70.
    // USCI_B_I2C_setSlaveAddress(USCI_B1_BASE, 0x70);
    // USCI_B_I2C_setMode(USCI_B1_BASE, USCI_B_I2C_TRANSMIT_MODE);

    // USCI_B_I2C_SENDING_START(0x70);   // LSB should be /WRITE



    // 写0x4c 到 pcf85176 模式寄存器？, 工作在 40*4 模式下
    // datasheet 6.1.1  Page-6
    // 0            10     0        1        1           00
    // follow data  fixed  unused   enable   bias = 1/2  multiplex=1:4
    //  0b0100,1100  == 0X4C


    // LCD 显示 "Hello" 
}

void Hmi::setup_keys(){
    // P1.1 is a button on launchpad board.
    // P1.0 is a LED on launchpad board.
    P1DIR = BIT0;   // P1.0 设置为输出, 其他的都是 INPUT

    P1REN = BIT1;   // enalbe P1.1 internal pull resistor. 
    P1OUT = BIT1;   // select P1.1 Pull-up, not pull-down.

    // P1REN = BIT4;   
    // P1OUT = BIT4;   

    // P1REN = BIT5;   
    // P1OUT = BIT5;   

    // P1REN = BIT6;   
    // P1OUT = BIT6;   

}

// 根据 显示的位置，和单一数字的数值， 返回一个码段
uint8_t get_segcode_from_digit(int position, int value){
    return 0x3f;
}
void print_int(int x){
    // 目标： 显示一个整数。
    // 步骤1: 把整数转换为 固定长度（包括空格）的字符串。 内部代码， 用 ASCII 编码？还是 4 bits 半字节? 
    uint8_t digits[] = {0,0,0,0};
    digits[0] = x % 1;    // 最右端   
    digits[1] = x % 10 / 10;
    digits[2] = x % 100 /100;
    digits[3] = x % 1000 / 1000 ;      // 最左端  将显示  0123


    
    // 步骤2： 查表获得 码值.  
    __uint8_t segcode[4];
    for (int i=0; i<4; i++){
        // 每一个数字有 7个segments, 这7个 segments 被分配到4个或者8个 pcf85176的 data buffer中。
        segcode[i] = get_segcode_from_digit(i, digits[i]);
    }
    

    // 每个位置（字符位置）获得码值可能会不同。
    // char* segs = from_str_x(str_x);


    // 步骤3： 通过 I2C 输出码值
    // Datasheet P29
    // chip_addr + command      + display_data
    // 1 byte      n>=1 bytes      n>=0 bytes
    // i2c.write_bytes(addr, reg_addr,bytes_length, segs);

}


