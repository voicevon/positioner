#include "adc12_a.h"
#include "driverlib.h"
#include "delay.h"
#include "gpio.h"
#include "timer_a.h"
#include "ucs.h"

extern unsigned long adc_result[2];


#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR (void)
{
    int irq_id = __even_in_range(ADC12IV, 34);
    // switch (__even_in_range(ADC12IV, 34))
    switch (irq_id)
    {
        case  0: break;   //Vector  0:  No interrupt
        case  2: break;   //Vector  2:  ADC overflow
        case  4: break;   //Vector  4:  ADC timing overflow
        case  6:   
        {
		    adc_result[0] = ADC12_A_getResults(ADC12_A_BASE, ADC12_A_MEMORY_0);
            break;
        }		 	 //Vector  6:  ADC12IFG0
        case  8:
        {
            adc_result[1] = ADC12_A_getResults(ADC12_A_BASE, ADC12_A_MEMORY_1);

            // 退出休眠模式，主程序可以继续
            // __bic_SR_register_on_exit(LPM0_bits);

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


// https://blog.csdn.net/weixin_44457994/article/details/119095625?spm=1001.2101.3001.6650.18&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7Ebaidujs_utm_term%7ECtr-18-119095625-blog-131824423.235%5Ev43%5Epc_blog_bottom_relevance_base6&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7Ebaidujs_utm_term%7ECtr-18-119095625-blog-131824423.235%5Ev43%5Epc_blog_bottom_relevance_base6&utm_relevant_index=22
void init_CLOCK(void)
{
    // 设置电源模式 。 本模式可以 enable mcu 工作在 25Mhz.  
    //高主频工作需要较高的核心电压
    PMM_setVCore(PMM_CORE_LEVEL_3);

    // XT1引脚复用  32768Hz  P5.4,   P5.5
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, (GPIO_PIN4 | GPIO_PIN5));
    //起振XT1, 是否在浪费能源 ？
    UCS_turnOnLFXT1(UCS_XT1_DRIVE_0, UCS_XCAP_3);

    // XT2 引脚复用 4MHz P5.2,   P5.3
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, (GPIO_PIN2 | GPIO_PIN3));
    //起振XT2
    UCS_turnOnXT2(UCS_XT2_DRIVE_4MHZ_8MHZ);

    // 设置两个外部晶振的频率，这里不是启动。仅仅是一个设置，以后可以用库来计算和频率相关的数值。
    // 设置外部时钟源的频率，使得在调用UCS_getMCLK, UCS_getSMCLK 或 UCS_getACLK时可得到正确值
    UCS_setExternalClockSource(XT1_FREQ,    // 32768 Hz
                               XT2_FREQ);   // 4 Mhz

    //XT2作为FLL参考时钟，先8分频，再50倍频 4MHz / 8 * 50 = 25MHz ??
    UCS_initClockSignal(UCS_FLLREF, UCS_XT2CLK_SELECT, UCS_CLOCK_DIVIDER_8);
    UCS_initFLLSettle(25000, 50);

    // UCS_initClockSignal(UCS_FLLREF, UCS_XT2CLK_SELECT, UCS_CLOCK_DIVIDER_4);
    // UCS_initFLLSettle(MCLK_KHZ, MCLK_FLLREF_RATIO);

    //DCOCLK作为SMCLK时钟源 = 25MHz ？
    // SMCLK 分配到哪里？   TIMER_B_PWM，  
    UCS_initClockSignal(UCS_SMCLK, UCS_XT2CLK_SELECT, UCS_CLOCK_DIVIDER_2);

    //XT1作为ACLK时钟源 = 32768Hz
    // ACLK 分配到哪里？  ADC12   TIMER_1_A1定时器
    UCS_initClockSignal(UCS_ACLK, UCS_XT1CLK_SELECT, UCS_CLOCK_DIVIDER_1);
}

void init_GPIO(void)
{
    // 开发板的 LED 输出
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN7);

    // 键盘输入
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 |GPIO_PIN5);

    // PWM 输出,  P3.5  Motor
    // P3.6  PWM 输出是 Pin 的第二功能
    // GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,GPIO_PIN2 + GPIO_PIN3);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,GPIO_PIN5 + GPIO_PIN6);

    // ADC 输入  P6.0,  P6.1  ，ADC 输入是 Pin 的第二功能
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0 | GPIO_PIN1);
}

void init_ADC_REF(void)
{
    while(REF_ACTIVE == Ref_isRefGenBusy(REF_BASE)) {};

    Ref_setReferenceVoltage(REF_BASE, REF_VREF1_5V);
    Ref_enableReferenceVoltage(REF_BASE);
    __delay_cycles(100);
}

void init_ADC12(void)
{
    ADC12_A_configureMemoryParam CH0_configureMemoryParam = {0};
    ADC12_A_configureMemoryParam CH1_configureMemoryParam = {0};

    CH0_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_0;
    CH0_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A0;  // P6.0  Feedback
    CH0_configureMemoryParam.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_INT;     // 采用 AVCC作为最大值，可能是数值不稳定的原因
    CH0_configureMemoryParam.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
    CH0_configureMemoryParam.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;

    CH1_configureMemoryParam.memoryBufferControlIndex = ADC12_A_MEMORY_1;
    CH1_configureMemoryParam.inputSourceSelect = ADC12_A_INPUT_A1;  // P6.1  Target, from 4-20mA 电流环  范围： 2.2V ~ 2.7V
    // CH1_configureMemoryParam.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_INT;
    CH1_configureMemoryParam.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
    // CH0_configureMemoryParam.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
    CH1_configureMemoryParam.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_EXT;  // 外接2V作为参考电压。
    CH1_configureMemoryParam.endOfSequence = ADC12_A_ENDOFSEQUENCE;   // 本通道是多通道里的最后一个。

    ADC12_A_init(ADC12_A_BASE,
                 ADC12_A_SAMPLEHOLDSOURCE_SC,
                 ADC12_A_CLOCKSOURCE_ACLK,
                 ADC12_A_CLOCKDIVIDER_1);

    ADC12_A_setupSamplingTimer(ADC12_A_BASE,
                               ADC12_A_CYCLEHOLD_256_CYCLES,
                            //    ADC12_A_CYCLEHOLD_256_CYCLES,
                               ADC12_A_CYCLEHOLD_4_CYCLES,
                               ADC12_A_MULTIPLESAMPLESENABLE);

    ADC12_A_setResolution(ADC12_A_BASE, ADC12_A_RESOLUTION_12BIT);
    ADC12_A_configureMemory(ADC12_A_BASE, &CH0_configureMemoryParam);
    ADC12_A_configureMemory(ADC12_A_BASE, &CH1_configureMemoryParam);

    // 使用转换完中断。这样可以更及时的获得转换结果。
    // 这里不使用中断，可以进一步降低功耗。？？ 
    ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12IFG0);
    ADC12_A_enableInterrupt(ADC12_A_BASE, ADC12IE0);	
    ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12IFG1);
    ADC12_A_enableInterrupt(ADC12_A_BASE, ADC12IE1);
    // __enable_interrupt();

    ADC12_A_enable(ADC12_A_BASE);
    // 在主程序启动转换，每10ms转换一次。
    // ADC12_A_startConversion(ADC12_A_BASE, ADC12_A_MEMORY_0, ADC12_A_REPEATED_SEQOFCHANNELS);
}

// https://e2e.ti.com/support/microcontrollers/msp-low-power-microcontrollers-group/msp430/f/msp-low-power-microcontroller-forum/921298/msp430f5419a-msp430-driverlib-how-to-output-pwm-on-multiple-channels
void init_PWM_TIMER_B0(void){
    //Start timer
    Timer_B_initUpModeParam initUpParam = {0};
    initUpParam.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    initUpParam.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    initUpParam.timerPeriod = 16383;  // 14 bits
    initUpParam.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    initUpParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE;
    initUpParam.timerClear = TIMER_B_DO_CLEAR;
    initUpParam.startTimer = false;
    Timer_B_initUpMode(TIMER_B0_BASE, &initUpParam);

	Timer_B_startCounter(TIMER_B0_BASE,TIMER_B_UP_MODE);

	//Initialize compare mode to generate PWM1
    Timer_B_initCompareModeParam initComp1Param = {0};
    initComp1Param.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_5;    // P3.5
    initComp1Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp1Param.compareOutputMode = TIMER_B_OUTPUTMODE_RESET_SET;
    // 这个示例， compareValue 是固定的，如何 update it ?  Timer_B_setCompareValue()
    initComp1Param.compareValue = 1;
    Timer_B_initCompareMode(TIMER_B0_BASE, &initComp1Param);

	//Initialize compare mode to generate PWM2
    Timer_B_initCompareModeParam initComp2Param = {0};
    initComp2Param.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_6;     // P3.6
    initComp2Param.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp2Param.compareOutputMode = TIMER_B_OUTPUTMODE_RESET_SET;
    initComp2Param.compareValue = 1;
    Timer_B_initCompareMode(TIMER_B0_BASE, &initComp2Param);
	// __bis_SR_register(LPM0_bits + GIE);                 // CPU off
}

void init_timer_T1A1(void)
{
    // 时钟来源设置为 XT1 --> ACLK 32768 Hz, 每个 tick = 1/32768 s.
    // 计数到 32768 时， time_period = 1s,  
    // 计数到 327.68 时， time_period = 10ms. 
    Timer_A_initUpModeParam UpModeParam = {0};

    // UpModeParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;            // 4 MHz
    UpModeParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;             // 32768 Hz
    UpModeParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1; // 不分频
    UpModeParam.timerPeriod = 327;    // per 10 ms.
    UpModeParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    UpModeParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    UpModeParam.timerClear = TIMER_A_DO_CLEAR;
    UpModeParam.startTimer = true;

    Timer_A_initUpMode(TIMER_A1_BASE, &UpModeParam);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    // __enable_interrupt();
}


#include <string.h>
#include <stdarg.h>
#include <stdio.h>

void UART_printf(uint16_t baseAddress, const char *format,...)
{
    uint32_t length;
    va_list args;
    uint32_t i;
    char TxBuffer[128] = {0};

    va_start(args, format);
    length = vsnprintf((char*)TxBuffer, sizeof(TxBuffer)+1, (char*)format, args);
    va_end(args);

    for(i = 0; i < length; i++)
        USCI_A_UART_transmitData(baseAddress, TxBuffer[i]);
}


bool init_UART(uint16_t baseAddress, uint32_t Baudrate)
{
    float UART_Temp = 0;
    USCI_A_UART_initParam huart = {0};

    if(baseAddress == USCI_A0_BASE)         //P3.3, P3.4 = USCI_A0 TXD/RXD
    {
        GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3, GPIO_PIN3);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN4);
    }
    else if(baseAddress == USCI_A1_BASE)    //P4.4, P4.5 = USCI_A1 TXD/RXD
    {
        GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P4, GPIO_PIN4);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN5);
    }

    if(Baudrate <= 9600)
    {
        huart.selectClockSource = USCI_A_UART_CLOCKSOURCE_ACLK;
        UART_Temp = (float)UCS_getACLK()/Baudrate;
    }
    else
    {
        huart.selectClockSource = USCI_A_UART_CLOCKSOURCE_SMCLK;
        UART_Temp = (float)UCS_getSMCLK()/Baudrate;
    }

    if(UART_Temp < 16)
        huart.overSampling = USCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;
    else
    {
        huart.overSampling = USCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
        UART_Temp /= 16;
    }

    huart.clockPrescalar = (int)UART_Temp;

    if(huart.overSampling == USCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION)
    {
        huart.secondModReg = (int)((UART_Temp - huart.clockPrescalar) * 8);
    }
    else
    {
        huart.firstModReg = (int)((UART_Temp - huart.clockPrescalar) * 16);
    }

    huart.parity = USCI_A_UART_NO_PARITY;
    huart.msborLsbFirst = USCI_A_UART_LSB_FIRST;
    huart.numberofStopBits = USCI_A_UART_ONE_STOP_BIT;
    huart.uartMode = USCI_A_UART_MODE;

    if (STATUS_FAIL == USCI_A_UART_init(baseAddress, &huart))
    {
        return STATUS_FAIL;
    }

    //Enable UART module for operation
    USCI_A_UART_enable(baseAddress);

    //Enable Receive Interrupt
    USCI_A_UART_clearInterrupt(baseAddress, USCI_A_UART_RECEIVE_INTERRUPT);
    USCI_A_UART_enableInterrupt(baseAddress, USCI_A_UART_RECEIVE_INTERRUPT);

    return STATUS_SUCCESS;
}


//  ********************************************************
//      EEPROM,  read, erase, write

#define INFOA_START     0x1980
#define INFOA_END       0x19FF

#define INFOB_START     0x1900
#define INFOB_END       0x197F

#define INFOC_START     0x1880
#define INFOC_END       0x18FF

#define INFOD_START     0x1800
#define INFOD_END       0x187F

// char Flash_Read_Char(unsigned int address);
// unsigned int Flash_Read_Word(unsigned int address);

extern void UART_printf(uint16_t baseAddress, const char *format,...);
char Flash_Read_Char(unsigned int address)
{
    char value = 0x00;
    char *FlashPtr = (char *)address;
    value = *FlashPtr;
    return value;
}

unsigned int Flash_Read_Word(unsigned int address)
{
    unsigned int value = 0x0000;
    unsigned int *FlashPtr = (unsigned int *)address;
    value = *FlashPtr;
    return value;
}

void write_eeprom(){
    unsigned char temp = 0x00;
    unsigned int status = 0x0000;
    unsigned char* set_values;
    // CRC 模块需要一个种子， 0xABCD是随机选择的一个种子值。
    CRC_setSeed(CRC_BASE, 0xABCD);
    CRC_set8BitData(CRC_BASE, Flash_Read_Char(INFOB_START));
    temp = CRC_getResult(CRC_BASE);
    if(temp != Flash_Read_Char(INFOB_START + 1))
    {
        // 擦除整个 Info B segment ， 共 128 bytes.
        do
        {
            FlashCtl_eraseSegment((unsigned char *)INFOB_START);
            status = FlashCtl_performEraseCheck((unsigned char *)INFOB_START, 128);
        }while(status == STATUS_FAIL);
    }
    //  FlashCtl_write8(set_values, (unsigned char *)INFOA_START, 4);
    //  FlashCtl_lockInfoA();

}








