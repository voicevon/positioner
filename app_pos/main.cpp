
#include "msp430f5529.h"
#include "stdio.h"
#include "driverlib.h"
#include "intrinsics.h"
#include "msp430f5xx_6xxgeneric.h"
#include "app_type.h"
#include "app_motor.h"
#include <cstdint>
// https://embeddedprojecthunter.wordpress.com/2015/08/28/simple-button-programming-for-msp430g2553/
// GPIO寄存器说明:   https://blog.csdn.net/qq_43935020/article/details/113201733  






// extern setup_keys();
#include "app_hmi.h"

Hmi my_hmi;
void setup_motor(){
    // 设置按钮
    // P1REN |= BIT1;  // P1.1 Enable pull-up or pull-down
    // P1OUT |= BIT1;  // P1.1 设置为输出模式

    P1DIR |= BIT2;  // P1.2 连接 LED, 高电平 亮
    P1SEL |= BIT2;  // 设置 P1.2 工作在 PWM 输出. TA0固定连接到 P1.2
    
    // 设置 PWM 工作模式
    TA0CCR0 = 999;    //设置 PWM 周期, 时钟源还没有设置呀！？
    TA0CCTL1 = OUTMOD_7;  // PWM 工作在 Reset/Set模式
    TA0CCR1 = 0;     // 初始化占空比 = 0?
    TA0CTL |= TASSEL_1 + MC_1;  // 设置时钟源
}

void main_key_and_led (void){
    WDTCTL = WDTPW + WDTHOLD;   // Disable watchdog.
    // setup_keys();
    my_hmi.setup_keys();
    while (1) {
        uint8_t p1_input = P1IN;
        if ((p1_input & BIT1) == BIT1){
            // button is presseed
            // _delay_cycles(20000000);
            // P1OUT ^= BIT0;    // turn led inversed.  
            P1OUT = 0xfe;   // 板上 LED 低电平 熄灭. 注意，仅输出 bit0.
        }else {
            P1OUT = 0xff;  // 板上 LED 高电平 点亮。
        }
        // printf(".");
    }
}

void main(void){
    WDTCTL = WDTPW + WDTHOLD;
    setup_motor();
    uint16_t ccr1;
    while(1){
        _delay_cycles((20000));
        ccr1 = TA0CCR1;
        ccr1 = ccr1 + 10;
        // TA0CCR1 = TA0CCR1 + 100;    //增加占空比？
        TA0CCR1 = ccr1;
        if (TA0CCR1 > 999){
            TA0CCR1 = 0;
            printf("hello");
        }

    }
}
