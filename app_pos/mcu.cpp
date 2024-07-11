#include "driverlib.h"
#include "delay.h"
#include "gpio.h"

extern unsigned long adc_result[2];



#pragma vector = ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
    switch (__even_in_range(ADC12IV, 34))
    {
        case  0: break;   //Vector  0:  No interrupt
        case  2: break;   //Vector  2:  ADC overflow
        case  4: break;   //Vector  4:  ADC timing overflow
        case  6:   
        {
		adc_result[0] = ADC12_A_getResults(ADC12_A_BASE,
                                        ADC12_A_MEMORY_0);
            break;
        }		 	 //Vector  6:  ADC12IFG0
        case  8:
        {
            adc_result[1] = ADC12_A_getResults(ADC12_A_BASE,
                                        ADC12_A_MEMORY_1);
            break;
        }                 //Vector  8:  ADC12IFG1
        case 10: break;   //Vector 10:  ADC12IFG2
        case 12: break;   //Vector 12:  ADC12IFG3
        case 14: break;   //Vector 14:  ADC12IFG4
        case 16: break;   //Vector 16:  ADC12IFG5
        case 18: break;   //Vector 18:  ADC12IFG6
        case 20: break;   //Vector 20:  ADC12IFG7
        case 22: break;   //Vector 22:  ADC12IFG8
        case 24: break;   //Vector 24:  ADC12IFG9
        case 26: break;   //Vector 26:  ADC12IFG10
        case 28: break;   //Vector 28:  ADC12IFG11
        case 30: break;   //Vector 30:  ADC12IFG12
        case 32: break;   //Vector 32:  ADC12IFG13
        case 34: break;   //Vector 34:  ADC12IFG14
        default: break;
    }
}



void init_CLOCK(void)
{
    // 设置电源模式 。 本模式可以 enable mcu 工作在 25Mhz.  
    PMM_setVCore(PMM_CORE_LEVEL_3);

    // ??
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5,
                                               (GPIO_PIN4 | GPIO_PIN2));
    // ??
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,
                                                (GPIO_PIN5 | GPIO_PIN3));

    // 设置两个外部晶振的频率，这里不是启动。仅仅是一个设置，以后可以用库来计算和频率相关的数值。
    UCS_setExternalClockSource(XT1_FREQ,    // 32768 Hz
                               XT2_FREQ);   // 4 Mhz

    UCS_turnOnXT2(UCS_XT2_DRIVE_4MHZ_8MHZ);

    UCS_turnOnLFXT1(UCS_XT1_DRIVE_0,
                    UCS_XCAP_3);

    UCS_initClockSignal(UCS_FLLREF,
                        UCS_XT2CLK_SELECT,
                        UCS_CLOCK_DIVIDER_4);

    UCS_initFLLSettle(MCLK_KHZ,
                      MCLK_FLLREF_RATIO);

    UCS_initClockSignal(UCS_SMCLK,
                        UCS_XT2CLK_SELECT,
                        UCS_CLOCK_DIVIDER_2);

    UCS_initClockSignal(UCS_ACLK,
                        UCS_XT1CLK_SELECT,
                        UCS_CLOCK_DIVIDER_1);
}

void init_GPIO(void)
{
    // 开发板的 LED 输出
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN7);
    // 键盘输入
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 |GPIO_PIN5);

    // PWM 输出, PWM 输出是 Pin 的第二功能
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,GPIO_PIN2 + GPIO_PIN3);

    // ADC 输入，ADC 输入是 Pin 的第二功能
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0 | GPIO_PIN1);
}

void init_ADC12(void)
{
    ADC12_A_configureMemoryParam CH0_configureMemoryParam = {0};
    ADC12_A_configureMemoryParam CH1_configureMemoryParam = {0};

    CH0_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_0;
    CH0_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A0;
    CH0_configureMemoryParam.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
    CH0_configureMemoryParam.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
    CH0_configureMemoryParam.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;

    CH1_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_1;
    CH1_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A1;
    CH1_configureMemoryParam.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
    CH1_configureMemoryParam.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
    CH1_configureMemoryParam.endOfSequence = ADC12_A_ENDOFSEQUENCE;

    ADC12_A_init(ADC12_A_BASE,
                 ADC12_A_SAMPLEHOLDSOURCE_SC,
                 ADC12_A_CLOCKSOURCE_ACLK,
                 ADC12_A_CLOCKDIVIDER_1);

    ADC12_A_setupSamplingTimer(ADC12_A_BASE,
                               ADC12_A_CYCLEHOLD_256_CYCLES,
                               ADC12_A_CYCLEHOLD_4_CYCLES,
                               ADC12_A_MULTIPLESAMPLESENABLE);

    ADC12_A_setResolution(ADC12_A_BASE,
                          ADC12_A_RESOLUTION_12BIT);

    ADC12_A_configureMemory(ADC12_A_BASE,
                            &CH0_configureMemoryParam);

    ADC12_A_configureMemory(ADC12_A_BASE,
                            &CH1_configureMemoryParam);

    ADC12_A_clearInterrupt(ADC12_A_BASE,
                           ADC12IFG0);

    ADC12_A_enableInterrupt(ADC12_A_BASE,
                            ADC12IE0);	

    ADC12_A_clearInterrupt(ADC12_A_BASE,
                           ADC12IFG1);

    ADC12_A_enableInterrupt(ADC12_A_BASE,
                            ADC12IE1);

    __enable_interrupt();

    ADC12_A_enable(ADC12_A_BASE);

    ADC12_A_startConversion(ADC12_A_BASE,
                            ADC12_A_MEMORY_0,
                            ADC12_A_REPEATED_SEQOFCHANNELS);
}


void init_PWM_TIMER_A0(void){
    Timer_A_initCompareModeParam CompareModeParam1 = {0};
    Timer_A_initCompareModeParam CompareModeParam2 = {0};
    // Timer_A_initCompareModeParam CompareModeParam3 = {0};

    Timer_A_initContinuousModeParam ContinuousModeParam = {0};

    ContinuousModeParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    ContinuousModeParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    ContinuousModeParam.timerClear = TIMER_A_DO_CLEAR;
    ContinuousModeParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    ContinuousModeParam.startTimer = false;

    CompareModeParam1.compareInterruptEnable = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    CompareModeParam1.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    CompareModeParam1.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    CompareModeParam1.compareValue = 0;

    CompareModeParam2.compareInterruptEnable = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    CompareModeParam2.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    CompareModeParam2.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    CompareModeParam2.compareValue = 0;

    // CompareModeParam3.compareInterruptEnable = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    // CompareModeParam3.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    // CompareModeParam3.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;
    // CompareModeParam3.compareValue = 0;

    Timer_A_initCompareMode(TIMER_A0_BASE, &CompareModeParam1);

    Timer_A_initCompareMode(TIMER_A0_BASE, &CompareModeParam2);

    // Timer_A_initCompareMode(TIMER_A0_BASE, &CompareModeParam3);

    Timer_A_initContinuousMode(TIMER_A0_BASE, &ContinuousModeParam);

    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);
}

