#pragma once

#include <boost/utility.hpp>

#include "MotionTracker/StartGestureData.h"

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class StartTempoEstimator : public boost::noncopyable
{
public:
	StartTempoEstimator();

	void SetStartTempo(tempo_t const & tempoInScore) { tempoInScore_ = tempoInScore; }

	void RegisterStartGesture(MotionTracker::StartGestureData const & data);

	real_time_t StartTimeEstimate() const;
	tempo_t TempoEstimate() const;
	speed_t SpeedEstimate() const;

private:
	bool ReadyForEstimates() const;
	tempo_t TempoFromStartGesture() const;

private:
	tempo_t tempoInScore_;

	real_time_t startTimeEstimate_;
	tempo_t tempoEstimate_;
};

} // namespace ScoreFollower
} // namespace cf
