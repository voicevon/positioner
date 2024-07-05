#include "app_hmi.h"
#include "driverlib.h"
#include "intrinsics.h"
#include "msp430f5xx_6xxgeneric.h"


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
