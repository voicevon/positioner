
#pragma once
#include "../base/filter_base.h"

namespace signalx{

class Ax_B : public FilterBase {
	public:
		explicit Ax_B(double slope, double offset);
        void Reset() override;
		void SetKnownPoint(double x, double y) override;

	protected:
		int ProcessSignal_(int value) override;

    private:
        double slope__ = 1.0;
        double offset__ = 0.0;
        double known_x1 = NAN;
        double known_y1 = NAN;
        double known_x2 = NAN;
        double known_y2 = NAN;

};


}   //  namespace
