#pragma once

#include <boost/utility.hpp>

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class StartTempoEstimator : public boost::noncopyable
{
public:
	StartTempoEstimator();

	void SetStartTempo(tempo_t const & tempoInScore) { tempoInScore_ = tempoInScore; }

	void RegisterStartGestureLength(duration_t const & gestureDuration);
	void RegisterPreparatoryBeat(real_time_t const & time);

	bool ReadyForEstimates();
	real_time_t StartTimeEstimate();
	speed_t SpeedEstimate();

private:
	tempo_t TempoFromStartGesture();

private:
	tempo_t tempoInScore_;
	duration_t startGestureDuration_;
	real_time_t preparatoryBeatTime_;
};

} // namespace ScoreFollower
} // namespace cf
