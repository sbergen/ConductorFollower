#include "MotionFilter.h"

#include "cf/polynomial.h"

namespace cf {
namespace MotionTracker {


MotionFilter::MotionFilter()
	: filter_(1.0 / FrameRateDependent::frame_rate)
{
}

bool
MotionFilter::NewPosition(timestamp_t const & time, Point3D const & pos)
{
	static int positionsReceived = 0;

	filter_.AppendValue(pos.data());
	if (++positionsReceived <= FrameRateDependent::filter_size) { return false; }

	RunFilter();
	return true;
}

void
MotionFilter::RunFilter()
{
	filter_.RunFromValues();
	EvaluateCoefs();
}

void
MotionFilter::EvaluateCoefs()
{
	filter_.EvaluateDerivative<0>(motionState_.position.data());
	filter_.EvaluateDerivative<1>(motionState_.velocity.data());
	filter_.EvaluateDerivative<2>(motionState_.acceleration.data());
	filter_.EvaluateDerivative<3>(motionState_.jerk.data());
}

} // namespace MotionTracker
} // namespace cf
