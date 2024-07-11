#include "filter_base.h"


namespace signalx {

void FilterBase::AppendFollower(FilterBase* followed_filter){
    this->followers__.push_back(followed_filter);
    // followed_filter->SetSignal(this->signal_);
}

void FilterBase::AppendFollower(const char* interest_result_item, FilterBase* filter){
    this->followers__.push_back(filter);
    // filter->SetSignal(this->signal_);  // filter will write back the result to
}


double FilterBase::Feed(double new_value){
    result_value_ = ProcessSignal_(new_value);
    return result_value_;
    // double master_result = result_value_;
    // for(auto &sub_filter : followers__){
    //     if (sub_filter->IsMasterUtility()){
    //         master_result = sub_filter->Feed(result_value_);
    //     }else{
    //         sub_filter->Feed(result_value_);
    //     }
    // }
    // return master_result;

}

// void FilterBase::SetSignal(SingleSignal* signal){
//     signal_ = signal;
//     SetSingalFilteredResult_();
// }


}  // namespace sensor
