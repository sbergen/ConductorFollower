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
StartTempoEstimator::RegisterStartGesture(MotionTracker::StartGestureData const & data)
{
	startGestureDuration_ = data.gestureDuration;
	preparatoryBeatTime_ = data.preparatoryBeatTime;
}

real_time_t
StartTempoEstimator::StartTimeEstimate() const
{
	if (!ReadyForEstimates()) { return real_time_t::max(); }

	return preparatoryBeatTime_ + time::duration_cast<duration_t>(1.0 * score::beats / TempoFromStartGesture());
}

tempo_t
StartTempoEstimator::TempoEstimate() const
{
	if (!ReadyForEstimates()) { return tempoInScore_; }
	auto tempo = TempoFromStartGesture();
	LOG("Starting tempo: %1%", tempo);
	return tempo;
}

speed_t
StartTempoEstimator::SpeedEstimate() const
{
	if (!ReadyForEstimates()) { return 1.0; }
	speed_t speed = TempoEstimate() / tempoInScore_;

	LOG("Starting speed_: %1%", speed);
	return speed;
}

bool
StartTempoEstimator::ReadyForEstimates() const
{
	return (startGestureDuration_ != duration_t(0) &&
	        preparatoryBeatTime_ != real_time_t::min());
}

tempo_t
StartTempoEstimator::TempoFromStartGesture() const
{
	return tempo_t(0.5 * score::beat / time::quantity_cast<time_quantity>(startGestureDuration_));
}

} // namespace ScoreFollower
} // namespace cf
