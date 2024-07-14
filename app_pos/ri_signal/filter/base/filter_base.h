#pragma once

// #include "ri_mcu/esp32/core/helpers.h"
// #include "ri_node_blue/nodes/base/nb_node_base.h"
#include "math.h"   // for NAN
#include <vector>
#include <queue>
#include <utility>	// for what?

namespace signalx {

class SingleSignal;

class FilterBase {
	public:
		int Feed(int value);
		virtual void Reset() = 0;
        void AppendFollower(FilterBase* filter);
        void AppendFollower(const char* interest_result_item, FilterBase* filter);
		bool IsMasterUtility(){return is_master_utility_;}
		void SetAsMasterUtility(){is_master_utility_ = true;}
		int GetResult(){return result_value_;}
		
		virtual void SetKnownPoint(double x, double y) = 0;


	protected:

		virtual int ProcessSignal_(int value) = 0;
		std::vector<FilterBase*> followers__;
		bool is_master_utility_ = false;
		int result_value_;
	private:
};


}  // namespace sensor
