// #pragma once

// #include "ri_signal/filter/base/filter_base.h"
// #include "ri_signal/filter/wave/wave.h"
// namespace signalx {


// class PIDController: public FilterBase{
//     public:
//         /**
//          *  
//          * @param P - Proportional gain 
//          * @param I - Integral gain
//          * @param D - Derivative gain 
//          * @param ramp - Maximum speed of change of the output value
//          * @param limit - Maximum output value
//          */
//         PIDController(double P, double I, double D, double ramp, double limit);
//         ~PIDController() = default;

//         // double operator() (double error);
//         double FeedError(double error);
//         double FeedBack(double sensor_value);
//         void SetNegativeDriving(){direction_is_positive_driving__ = false;}
//         void SetSP(double setpoint){setpoint__ = setpoint;}
//         double GetSP(){return setpoint__;}
//         double GetVelocity(){return velocity__;}
//         double GetError(){return error_now__;}
//         // double GetMaxOvershoot(){return max_overshoot__;}
//         double GetMaxEverPV(){return max_ever_pv__;}
//         void Reset() override;

//         void EnableWave_PV();
//         void EnableWave_Error();
//         WaveFilter* GetWave_PV() {return wave_pv__;}
//         WaveFilter* GetWave_Error(){return wave_error__;}
//         uint32_t GetUpTime_ms();
//         double Kp; //!< Proportional gain 
//         double Ki; //!< Integral gain 
//         double Kd; //!< Derivative gain 
//         double output_ramp; //!< Maximum speed of change of the output value
//         double limit; //!< Maximum output value
// 		void SetKnownPoint(double x, double y) override{}

//     protected:
// 		double ProcessSignal_(double value) override{return 0.0;}  // Feedback signal.  Not command signal

//         bool direction_is_positive_driving__ = true;
//         double setpoint__ = 0;
//         double velocity__ = 0;
//         double previouse_position__ = 0;
//         double error_now__ = 0;

//         double error_prev; //!< last tracking error value
//         double output_prev;  //!< last pid output value
//         double integral_prev; //!< last integral component value
//         unsigned long timestamp_prev; //!< Last execution timestamp

//         uint32_t process_count__ = 0;
//         uint32_t reset_timestamp__  =0;
//         // double max_overshoot__ = -999888;
//         double max_ever_pv__ = -999888;

//         WaveFilter *wave_pv__ = nullptr;
//         WaveFilter *wave_error__ = nullptr;
        
// };


// }   //  namespace