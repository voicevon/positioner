
// #include "pid.h"
// // #include "rios/console/console.h"
// // #include "esp_timer.h"


// #define _constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))


// namespace signalx {

// PIDController::PIDController(double P, double I, double D, double ramp, double limit)
//     : Kp(P)
//     , Ki(I)
//     , Kd(D)
//     , output_ramp(ramp)    // output derivative limit [volts/second]
//     , limit(limit)         // output supply limit     [volts]
//     , error_prev(0.0f)
//     , output_prev(0.0f)
//     , integral_prev(0.0f)
// {
//     timestamp_prev = esp_timer_get_time();
// }

// // PID controller function
// double PIDController::FeedBack(double pv){
//     if (wave_pv__ != nullptr){
//         wave_pv__->Feed(pv);
//     }
//     unsigned long timestamp_now = esp_timer_get_time();
//     // double Ts = (timestamp_now - timestamp_prev) * 1e-6f;
//     // // quick fix for strange cases (micros overflow)
//     // if(Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;
    
//     velocity__ = (pv - previouse_position__) / (timestamp_now - timestamp_prev);
//     previouse_position__ = pv; 
//     double error = pv - setpoint__;     //?  is direction correct?
//     bool debug = false;
//     if (debug){
//         Logger::Debug("PIDController::FeedBack() ");
//         Logger::Print("Kp", Kp);
//         Logger::Print("SP", setpoint__);
//         Logger::Print("PV ", pv);
//         Logger::Print("error", error);
        
//     }
//     return FeedError(error);
// }




// //https://stackoverflow.com/questions/15095206/what-does-an-operator-without-sign-and-two-sets-of-parameters-mean
// double PIDController::FeedError(double error){ 
//     if (wave_error__ != nullptr){
//         wave_error__->Feed(error);
//     } 
//     process_count__++;
//     // if (error > max_overshoot__) {
//     //     max_overshoot__ = error;
//     // }
//     if (error + setpoint__ > max_ever_pv__){
//         max_ever_pv__ = error + setpoint__;
//     }
//     error_now__ = error;
//     // calculate the time from the last call
//     unsigned long timestamp_now = esp_timer_get_time();
//     double Ts = (timestamp_now - timestamp_prev) * 1e-6f;
//     // quick fix for strange cases (micros overflow)
//     if(Ts <= 0 || Ts > 0.5f) {
//         Ts = 1e-3f;
//     }

//     // u(s) = (P + I/s + Ds)e(s)
//     // Discrete implementations
//     // proportional part
//     // u_p  = P *e(k)
//     double proportional = Kp * error;
//     // Tustin transform of the integral part
//     // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
//     double integral = integral_prev + Ki*Ts*0.5f*(error + error_prev);

//     // antiwindup - limit the output
//     integral = _constrain(integral, -limit, limit);
    
//     // Discrete derivation
//     // u_dk = D(ek - ek_1)/Ts
//     double derivative = Kd * (error - error_prev)/Ts;

//     // sum all the components
//     bool debug = false;
//     if (debug){
//         Logger::Print("PID  feed_error()  proportional ", proportional);
//         Logger::Print("PID  feed_error()  integral ", integral);
//         Logger::Print("PID  feed_error()  derivative ", derivative);
//     }
//     double output = proportional + integral + derivative;
//     // antiwindup - limit the output variable
//     output = _constrain(output, -limit, limit);


//     // if output ramp defined
//     if(output_ramp > 0){
//         // limit the acceleration by ramping the output
//         double output_rate = (output - output_prev)/Ts;
//         if (output_rate > output_ramp)
//             output = output_prev + output_ramp*Ts;
//         else if (output_rate < -output_ramp)
//             output = output_prev - output_ramp*Ts;
//     }
//     // saving for the next pass
//     integral_prev = integral;
//     output_prev = output;
//     error_prev = error;
//     timestamp_prev = timestamp_now;
//     if (direction_is_positive_driving__){
//         return -output;
//     }
//     return output;
// }

// void PIDController::EnableWave_PV(){
//     if (wave_pv__ == nullptr){
//         wave_pv__ = new WaveFilter();
//     }
// }

// void PIDController::EnableWave_Error(){
//     if (wave_error__ == nullptr){
//         wave_error__ = new WaveFilter();
//     }
// }

// void PIDController::Reset(){
//     process_count__ = 0;
//     reset_timestamp__ = esp_timer_get_time();
//     // max_overshoot__ = - 999888;
//     max_ever_pv__ = -999888;

//     integral_prev = 0.0f;
//     output_prev = 0.0f;
//     error_prev = 0.0f;

//     if (wave_pv__ != nullptr){
//         wave_pv__->Reset();
//     }
//     if (wave_error__ != nullptr){
//         wave_error__->Reset();
//     }
    
// }

// uint32_t PIDController::GetUpTime_ms(){
//     return (esp_timer_get_time() - reset_timestamp__) / 1000;
// }



// }   //  namespace
