#include "StartTempoEstimator.h"

#include "cf/globals.h"

namespace cf {
namespace ScoreFollower {

StartTempoEstimator::StartTempoEstimator()
	: startGestureDuration_(0)
	, preparatoryBeatTime_(real_time_t::min())
{
}

void
StartTempoEstimator::RegisterStartGestureLength(duration_t const & gestureDuration)
{
	startGestureDuration_ = gestureDuration;
}

void
StartTempoEstimator::RegisterPreparatoryBeat(real_time_t const & time)
{
	preparatoryBeatTime_ = time;
}

bool
StartTempoEstimator::ReadyForEstimates()
{
	return (startGestureDuration_ != duration_t(0) &&
	        preparatoryBeatTime_ != real_time_t::min());
}

real_time_t
StartTempoEstimator::StartTimeEstimate()
{
	if (!ReadyForEstimates()) { return real_time_t::max(); }

	return preparatoryBeatTime_ + time::duration_cast<duration_t>(1.0 * score::beats / TempoFromStartGesture());
}

speed_t
StartTempoEstimator::SpeedEstimate()
{
	if (!ReadyForEstimates()) { return 1.0; }
	speed_t speed = TempoFromStartGesture() / tempoInScore_;

	LOG("Starting tempo: %1%, speed_: %2%", TempoFromStartGesture(), speed);
	return speed;
}

tempo_t
StartTempoEstimator::TempoFromStartGesture()
{
	return tempo_t(0.5 * score::beat / time::quantity_cast<time_quantity>(startGestureDuration_));
}

} // namespace ScoreFollower
} // namespace cf
