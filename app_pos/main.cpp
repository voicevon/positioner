#include "driverlib.h"
#include "delay.h"
#include "timer_a.h"
// #include "lcd.h"
// #include "lcd_print.h"
// #include "mcu."
#include "./ri_signal/filter/formular/ax_b.h"
#include "./test/person.h"

extern void init_CLOCK(void);
extern void init_GPIO(void);
extern void init_ADC12(void);
extern void init_PWM_TIMER_A0(void);


unsigned long adc_result[2] ={0, 0};

Person me;
signalx::Ax_B my_filter(1, 0);

void main(void)
{
    WDT_A_hold(WDT_A_BASE);
    my_filter.Reset();
    my_filter.SetKnownPoint(0, 0);
    my_filter.SetKnownPoint(4096, 1.0);

    init_CLOCK();
    init_GPIO();
    init_ADC12();
    init_PWM_TIMER_A0();
    // LCD_init();
    // LCD_clear_home();

    // LCD_goto(0, 0);
    // LCD_putstr("CH0:");

    // LCD_goto(0, 1);
    // LCD_putstr("CH1:");


    while(1)
    {   

        Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, adc_result[1]);
        Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, adc_result[1]+ 1000);


        // 休眠，等待中断唤醒

        // delay_ms(100);
    };
}
