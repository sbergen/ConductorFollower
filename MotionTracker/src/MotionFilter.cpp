#include "MotionFilter.h"

#include "cf/polynomial.h"

namespace cf {
namespace MotionTracker {


MotionFilter::MotionFilter(InterThreadEventBuffer & eventBuffer)
	: eventBuffer_(eventBuffer)
	, positionBuffer_(FrameRateDependent::filter_size)
	, filter_(1.0 / FrameRateDependent::frame_rate)
{
}

void
MotionFilter::NewPosition(timestamp_t const & time, Point3D const & pos)
{
	positionBuffer_.RegisterEvent(time::now(), pos);
	filter_.AppendValue(pos.data());
	if (positionBuffer_.AllEvents().Size() < FrameRateDependent::filter_size) { return; }

	RunFilter();
	eventBuffer_.enqueue(Event(time, Event::MotionStateUpdate, motionState_));
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
