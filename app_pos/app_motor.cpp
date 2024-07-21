#include "app_motor.h"

#include "msp430f5529.h"
#include "timer_b.h"
#include <algorithm>
#include <vector>
#include "driverlib.h"

// #define FILE_NAME_ADC_AT_MIN "adc_min"
// #define FILE_NAME_ADC_AT_MAX "adc_max"

extern void UART_printf(uint16_t baseAddress, const char *format,...);
extern unsigned int Flash_Read_Word(unsigned int address);


void Motor::Dump(const char* title){
    UART_printf(USCI_A1_BASE, title);
    UART_printf(USCI_A1_BASE, "    adc= %d, velocity= %d  \n", this->current_adc, this->velocity);
}

#define INFOB_START     0x1900
Motor::Motor(){
    adc_at_position_min__ != Flash_Read_Word(INFOB_START);
    adc_at_position_max__ != Flash_Read_Word(INFOB_START+2);
}


#define FILTER_WINDOW_SIZE  5

void Motor::SetAdc(unsigned int adc_value){
    static int history_head = 0;
    static std::vector<int>  v_adc(FILTER_WINDOW_SIZE);  // 这个变量，无法被 调试程序  to watch.
    static previous_adc;   // 滤波之后的

    // 把 ADC 放入到历史队列中，用于计算 滤波 
    adc_history__[history_head] = adc_value;
    history_head++;
    if (history_head >= FILTER_WINDOW_SIZE){
        history_head = 0;
    }

    // 使用 中值滤波器， 窗口宽度= 5
    // std::vector<int> v_adc;
    for(int i=0; i < FILTER_WINDOW_SIZE; i++){
        // v_adc.push_back(adc_history__[i]);      //会使用 heap ， 430要小心
        v_adc[i] = adc_history__[i];
    }
    std::sort(v_adc.begin(), v_adc.end());

    int filtered_adc = v_adc[FILTER_WINDOW_SIZE / 2];
    this->current_adc = filtered_adc;
    this->PV = this->filter_adc_to_percent.Feed(filtered_adc);

    this->velocity = current_adc - previous_adc;

    previous_adc =  current_adc;

    // 这种算法， 最先的 10 次 velocity 是无效的，因为没有初始化 adc_history[10];
    // int history_tail = history_head + 1;
    // if (history_tail >=10){
    //     history_tail = 0;
    // }    
    // this->velocity = adc_value - adc_history__[history_tail];

    // for temp debug
    // if ((velocity > 100 ) || (velocity < -100)){
    //     UART_printf(USCI_A1_BASE, "xx\n");
    // }


}

int Motor::GetVelocity(){
    return this->velocity;
}
int Motor::GetSpeed(){
    return abs(this->velocity);
}

void Motor::SetForce(unsigned int force){
    // Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, force);
    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_5, force);    // P3.5 pwm output
}
void Motor::ForceUp(int offset){
    force += offset;
    // overflow issue ?
    this->SetForce(force);
}

void Motor::SaveForce_ForwardMin(){
    // printf("SaveForce_ForwardMin() ....");
}
void Motor::SaveForce_BackwardMax(){
    // printf("SaveForce_BackwardMax()");
}
void Motor::SavePosition_Min(){
    adc_at_position_min__ = this->current_adc;
}
void Motor::SavePosition_Max(){
    adc_at_position_max__ = this->current_adc;
}

void Motor::UpdateFilter(){
    filter_adc_to_percent.SetKnownPoint(adc_at_position_min__, 0.0);
    filter_adc_to_percent.SetKnownPoint(adc_at_position_max__, 1.0);
}
void Motor::SaveToFile(){
    // filex::FileX::NVS_Write(FILE_NAME_ADC_AT_MIN, adc_at_position_min__);
    // filex::FileX::NVS_Write(FILE_NAME_ADC_AT_MAX, adc_at_position_max__);
}
