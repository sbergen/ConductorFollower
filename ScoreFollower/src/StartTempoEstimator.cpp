#include "StartTempoEstimator.h"

#include <boost/units/cmath.hpp>

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

	return preparatoryBeatTime_ + time_cast<duration_t>(1.0 * score::beats / TempoFromStartGesture());
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
	// TODO take into account other beat patterns also?
	auto duration = time_cast<time_quantity>(startGestureDuration_);
	auto eightBeatTempo = tempo_t(0.5 * score::beat / duration);
	auto quarterBeatTempo = tempo_t(1.0 * score::beat / duration);

	auto eightDiff = bu::abs(eightBeatTempo - tempoInScore_);
	auto quarterDiff = bu::abs(quarterBeatTempo - tempoInScore_);

	return (eightDiff <= quarterDiff) ? eightBeatTempo : quarterBeatTempo;
}

} // namespace ScoreFollower
} // namespace cf
