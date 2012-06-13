#include "TempoFollower.h"

#include <boost/range/adaptors.hpp>

#include "globals.h"
#include "TimeWarper.h"

namespace cf {
namespace ScoreFollower {

TempoFollower::TempoFollower(TimeWarper const & timeWarper)
	: timeWarper_(timeWarper)
	, tempoMap_()
	, beatHistory_(100) // Arbitrary length, should be long enough...
	, newBeats_(false)
	, speed_(1.0)
{
}

void
TempoFollower::RegisterBeat(real_time_t const & beatTime)
{
	assert(beatTime > beatHistory_.AllEvents().LastTimestamp());

	// Beats will probably include a probability later...
	beatHistory_.RegisterEvent(beatTime, 1.0);
	newBeats_ = true;
}


speed_t
TempoFollower::SpeedEstimateAt(real_time_t const & time)
{
	if (!newBeats_) { return speed_; }
	newBeats_ = false;

	// Ignore beginning for now...
	if (beatHistory_.AllEvents().Size() < 16) { return speed_; }

	score_time_t scoreTime = timeWarper_.WarpTimestamp(time);
	TempoPoint tempoNow = tempoMap_.GetTempoAt(scoreTime);

	speed_t tempoSpeed = SpeedFromConductedTempo(tempoNow, time);	
	speed_t phaseSpeed = SpeedFromBeatCatchup(tempoNow, 1.0);
	speed_ = 0.4 * tempoSpeed + 0.6 * phaseSpeed;

	return speed_;
}

speed_t
TempoFollower::SpeedFromConductedTempo(TempoPoint const & tempoNow, real_time_t const & now) const
{
	tempo_t conductedTempo = BeatLengthEstimate();
	return static_cast<speed_t>(tempoNow.tempo().count()) / conductedTempo.count();
}

speed_t
TempoFollower::SpeedFromBeatCatchup(TempoPoint const & tempoNow, beat_pos_t catchupTime) const
{
	score_time_t catchup = time::multiply(tempoNow.tempo(), BeatOffsetEstimate());
	tempo_t newTempo = tempoNow.tempo() + time::divide(catchup, catchupTime);
	return static_cast<speed_t>(tempoNow.tempo().count()) / newTempo.count();
}

beat_pos_t
TempoFollower::BeatOffsetEstimate() const
{
	boost::array<double, 4> weights = { 0.5, 0.25, 0.15, 0.1 };

	auto beats = beatHistory_.AllEvents().timestampRange() | boost::adaptors::reversed;
	assert(beats.size() > 4);

	beat_pos_t weightedSum = 0.0;
	auto bIt = beats.begin();
	for(auto wIt = weights.begin(); wIt != weights.end(); ++wIt, ++bIt) {
		score_time_t beatScoreTime = timeWarper_.WarpTimestamp(*bIt);
		TempoPoint tempoPoint = tempoMap_.GetTempoAt(beatScoreTime);
		weightedSum += *wIt * tempoPoint.warpedFraction();
	}

	return weightedSum;
}

tempo_t
TempoFollower::BeatLengthEstimate() const
{
	boost::array<double, 4> weights = { 0.5, 0.25, 0.15, 0.1 };

	auto beats = beatHistory_.AllEvents().timestampRange() | boost::adaptors::reversed;
	assert(beats.size() > 4);

	duration_t weightedSum = duration_t::zero();
	auto bIt = beats.begin();
	real_time_t laterBeat = *bIt;
	++bIt;
	for(auto wIt = weights.begin(); wIt != weights.end(); ++wIt, ++bIt) {
		weightedSum += time::multiply(laterBeat - *bIt, *wIt);
		laterBeat = *bIt;
	}

	// Cast down to whole units (micro, nano, whatever, don't care about rounding...)
	return time::duration_cast<tempo_t>(weightedSum);
}

} // namespace ScoreFollower
} // namespace cf
