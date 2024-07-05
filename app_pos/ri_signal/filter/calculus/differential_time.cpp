
// #include "differential_time.h"
// // #include "rios/console/console.h"
// // #include "esp_timer.h"  // for esp_timer_get_time()

// namespace signalx{


// double DifferentTime::ProcessSignal_(double value) {
//     if (feed_count__ < 1) {
//         previous_timestamp__ = esp_timer_get_time();  //   micros();
//         previous_position__ = value;
//         result_value_ = 0.0;
//         feed_count__++;
//         return result_value_;
//     }

//     uint32_t current_timestamp = esp_timer_get_time();   //micros();
//     double current_speed = (value - previous_position__) / (current_timestamp - previous_timestamp__) * 1000000;
//     result_value_ = current_speed * sensitivity__ + result_value_ * (1.0 - sensitivity__);

//     // bool debug= true;
//     // if (debug){
//     //     Logger::Print("sensitivity__", sensitivity__);
//     //     Logger::Print("previous_position__", previous_position__);
//     //     Logger::Print("current_position", value);
//     //     Logger::Print("previous_timestamp__", previous_timestamp__);
//     //     Logger::Print("current_timestamp", current_timestamp);
//     //     Logger::Print("current_speed", current_speed);
//     //     Logger::Print("result_value_", result_value_);
//     // }
//     previous_timestamp__ = current_timestamp;
//     previous_position__ = value;
//     return result_value_;
// }

// DifferentTime::DifferentTime(double sensitivity){
//     sensitivity__ = sensitivity;
//     feed_count__ = 0;
//     // previous_timestamp__ = esp_timer_get_time();  //micros();
// }

// void DifferentTime::Reset(){

// }

// }   //  namespace
