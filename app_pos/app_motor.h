#pragma once
#include "./ri_signal/filter/formular/ax_b.h"
#include "./ri_signal/filter/calculus/differential_time.h"
// #include "ri_mcu_gpio/gpio_pwm.h"
// #include "ri_file/file_x.h"




class Motor{
    public:
        Motor();
        int PV;
        void SetAdc(unsigned int adc_value);
        int GetSpeed();
        int GetVelocity();
        void SetForce(unsigned force);
        void ForceUp(int offset);

        void SaveForce_ForwardMin();
        void SaveForce_BackwardMax();
        void SavePosition_Min();
        void SavePosition_Max();

        void UpdateFilter();
        void SaveToFile();

        void Dump(const char* title);
        

    private:
        signalx::Ax_B  filter_adc_to_percent = signalx::Ax_B(1.0, 0);
        int velocity;
        unsigned int force;
        unsigned int adc_at_position_min__ = 0;
        unsigned int adc_at_position_max__ = 0;
        unsigned int current_adc = 0;

        // ADC 历史队列，用于计算 velocity.
        int adc_history__[10];
        // // 最新采集到的 ADC
        // int history_head = 0;

};
