#include "StartTempoEstimator.h"

#include "cf/globals.h"

namespace cf {
namespace ScoreFollower {

StartTempoEstimator::StartTempoEstimator()
	: done_(false)
{
}

void
StartTempoEstimator::RegisterPreparatoryBeat(real_time_t const & time)
{
	preparatoryBeatTime_ = time;
}

speed_t
StartTempoEstimator::SpeedFromBeat(real_time_t const & beatTime, double /*clarity*/)
{
	// Only use the first "real" beat
	done_ = true;

	time_quantity beatDiff = time::quantity_cast<time_quantity>(beatTime - preparatoryBeatTime_);
	tempo_t conductedTempo(1.0 * score::beat / beatDiff);
	speed_t speed = conductedTempo / tempoInScore_;

	LOG("Starting tempo: %1%, (%3% - %4%) speed_: %2%", conductedTempo, speed, preparatoryBeatTime_, beatTime);
	return speed;
}

} // namespace ScoreFollower
} // namespace cf
