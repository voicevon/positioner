
#include "ax_b.h"

namespace signalx{


void Ax_B::Reset(){
    known_x1 = NAN;
    known_y1 = NAN;
    known_x2 = NAN;
    known_y2 = NAN;
}

double Ax_B::ProcessSignal_(double x) {
    result_value_ = slope__ * x + offset__;
    return result_value_;
}

void Ax_B::SetKnownPoint(double x, double y){
    // Logger::Print("Ax_B::SetKnownPoint(x) ", x );
    // Logger::Print("Ax_B::SetKnownPoint(y) ", y );
    // x2 should be greater than x1
    if (isnan(known_x1)){
        known_x1 = x;
        known_y1 = y;
    }else if (isnan(known_x2)){
        known_x2 = x;
        known_y2 = y;
        if (known_x2 < known_x1){
            known_x2 = known_x1;
            known_y2 = known_y1;
            known_x1 = x;
            known_y1 = y;
        }
    }else{
        if (x < known_x1){
            known_x1 = x;
            known_y1 = y;
        }
        if (x > known_x2){
            known_x2 = x;
            known_y2= y;
        }
    }

    if (! isnan(known_x2)){
        slope__ = (known_y2 - known_y1) / (known_x2 - known_x1);
        offset__ = known_y1 - slope__ * known_x1;


    }else{
        // Logger::Error("filter:: Ax_B::SetKnownPoint()   isnan(known_x2) is true! ");
        // Logger::Halt("");
    }
}


Ax_B::Ax_B(double slope, double offset){
    slope__ = slope;
    offset__ = offset;
}

}   //  namespace
