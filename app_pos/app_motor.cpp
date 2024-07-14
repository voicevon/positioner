#include "app_motor.h"
#include "driverlib.h"
#include "timer_b.h"

#define FILE_NAME_ADC_AT_MIN "adc_min"
#define FILE_NAME_ADC_AT_MAX "adc_max"


Motor::Motor(){
    // int io_num = 26;   
	// unsigned int pwm_frequency = 200;
	// ledc_timer_bit_t pwm_resolution = LEDC_TIMER_14_BIT;
	// pwm_output__ = new gpio::GpioPwm(io_num);
	// pwm_output__->InitPwm(pwm_frequency, pwm_resolution);
}

void Motor::SetAdc(unsigned int adc_value){
    this->current_adc = adc_value;
    this->PV = this->filter_adc_to_percent.Feed(adc_value);
    this->velocity =  this->filter_dt.Feed(adc_value);
    // this->PV = 0.5;
}

int Motor::GetSpeed(){
    return abs(this->velocity);
}
int Motor::GetVelocity(){
    return this->velocity;
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
