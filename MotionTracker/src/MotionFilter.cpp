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

	RunFilter(time);
}

void
MotionFilter::RunFilter(timestamp_t const & time)
{
	filter_.RunFromValues();
	EvaluateCoefs();
	CommitEvents(time);
}

void
MotionFilter::EvaluateCoefs()
{
	filter_.EvaluateDerivative<0>(position_.data());
	filter_.EvaluateDerivative<1>(velocity_.data());
	filter_.EvaluateDerivative<2>(acceleration_.data());
	//filter_.EvaluateDerivative<3>(jerk_.data());
}

void
MotionFilter::CommitEvents(timestamp_t const & time)
{
	eventBuffer_.enqueue(Event(time, Event::Position, position_));
	eventBuffer_.enqueue(Event(time, Event::Velocity, velocity_));
	eventBuffer_.enqueue(Event(time, Event::Acceleration, acceleration_));
	//eventBuffer_.enqueue(Event(time, Event::Jerk, jerk_));
}

} // namespace MotionTracker
} // namespace cf
