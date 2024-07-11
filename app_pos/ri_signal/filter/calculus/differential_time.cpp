
#include "differential_time.h"
// #include "rios/console/console.h"
// #include "esp_timer.h"  // for esp_timer_get_time()

namespace signalx{


double DifferentTime::ProcessSignal_(double value) {
    if (feed_count__ < 1) {
        // previous_timestamp__ = esp_timer_get_time();  //   micros();
        previous_position__ = value;
        result_value_ = 0.0;
        feed_count__++;
        return result_value_;
    }

    // uint32_t current_timestamp = esp_timer_get_time();   //micros();
    // double current_speed = (value - previous_position__) / (current_timestamp - previous_timestamp__) * 1000000;
    double current_speed = (value - previous_position__) / 0.01;   // fixed 10ms 
    result_value_ = current_speed * sensitivity__ + result_value_ * (1.0 - sensitivity__);

    // previous_timestamp__ = current_timestamp;
    previous_position__ = value;
    return result_value_;
}

DifferentTime::DifferentTime(double sensitivity){
    sensitivity__ = sensitivity;
    feed_count__ = 0;
    // previous_timestamp__ = esp_timer_get_time();  //micros();
}

void DifferentTime::Reset(){

}

}   //  namespace
