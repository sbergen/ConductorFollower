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
	void RegisterPreparatoryBeat(real_time_t const & time);
	speed_t SpeedFromBeat(real_time_t const & beatTime, double clarity);

	bool Done() { return done_; }

private:
	tempo_t tempoInScore_;
	real_time_t preparatoryBeatTime_;

	bool done_;
};

} // namespace ScoreFollower
} // namespace cf
