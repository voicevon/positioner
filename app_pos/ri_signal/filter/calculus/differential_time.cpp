
#include "differential_time.h"
// #include "rios/console/console.h"
// #include "esp_timer.h"  // for esp_timer_get_time()

namespace signalx{


// 速度， 应用于： Motor.Velocity

int DifferentTime::ProcessSignal_(int position) {
    if (feed_count__ < 1) {
        previous_position__ = position;
        result_value_ = 0.0;
        feed_count__++;
        return result_value_;
    }

    int current_speed = position - previous_position__;    // fixed delta time 10ms. 
    result_value_ = (int) (sensitivity__ * current_speed  + (1.0 - sensitivity__) * result_value_);
    // 仅仅为了观察数据，调试使用
    if (result_value_ > 2) {
        previous_position__ = position;
    }
    
    previous_position__ = position;
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
