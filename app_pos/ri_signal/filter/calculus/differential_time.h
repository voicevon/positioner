
#pragma once
#include "ri_signal/filter/base/filter_base.h"



namespace signalx{


class DifferentTime : public FilterBase {
	public:
		DifferentTime(double sensitivity);
        void Reset();
		void SetKnownPoint(double x, double y) override{};

	protected:
		int ProcessSignal_(int value) override;

    private:
		double sensitivity__ = 1;
		int feed_count__ = 0;
		// uint32_t previous_timestamp__;
		int previous_position__;
		// double speed__;

};


}   //  namespace
