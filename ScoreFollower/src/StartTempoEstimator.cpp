#include "StartTempoEstimator.h"

#include <boost/array.hpp>
#include <boost/units/cmath.hpp>

#include "cf/globals.h"

namespace cf {
namespace ScoreFollower {

StartTempoEstimator::StartTempoEstimator()
	: tempoInScore_(0.0 * score::beats_per_minute)
	, startTimeEstimate_(timestamp_t::max())
	, tempoEstimate_(0.0 * score::beats_per_minute)
{
}

void
StartTempoEstimator::RegisterStartGesture(MotionTracker::StartGestureData const & data)
{
	// TODO take into account other beat patterns also?
	boost::array<beats_t, 2> periods = { 1.0 * score::beats, 2.0 * score::beats };
	auto duration = time_cast<time_quantity>(data.gestureDuration);

	tempo_t bestTempo(0.0 * score::beats_per_minute);
	tempo_t bestDiff(std::numeric_limits<double>::max() * score::beats_per_minute);
	beats_t bestPeriod(0.0 * score::beats);
	std::for_each(std::begin(periods), std::end(periods),
		[&](beats_t const & period)
		{
			auto tempo = tempo_t(period / 2.0 / duration);
			auto diff = bu::abs(tempo - tempoInScore_);

			if (diff < bestDiff) {
				bestDiff = diff;
				bestTempo = tempo;
				bestPeriod = period;
			}
		});
	
	tempoEstimate_ = bestTempo;
	startTimeEstimate_ = data.preparatoryBeatTime + time_cast<duration_t>(bestPeriod / bestTempo);
}

real_time_t
StartTempoEstimator::StartTimeEstimate() const
{
	return startTimeEstimate_;
}

tempo_t
StartTempoEstimator::TempoEstimate() const
{
	return tempoEstimate_;
}

speed_t
StartTempoEstimator::SpeedEstimate() const
{
	if (!ReadyForEstimates()) { return 1.0; }
	speed_t speed = tempoEstimate_ / tempoInScore_;
	LOG("Starting speed_: %1%", speed);
	return speed;
}

bool
StartTempoEstimator::ReadyForEstimates() const
{
	return (tempoInScore_ != tempo_t(0.0 * score::beats_per_minute) &&
		startTimeEstimate_ != real_time_t::max());
}

} // namespace ScoreFollower
} // namespace cf
