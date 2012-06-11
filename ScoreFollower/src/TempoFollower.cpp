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

	speed_ = SpeedFromConductedTempo(time);	

	return speed_;
}

speed_t
TempoFollower::SpeedFromConductedTempo(real_time_t const & now) const
{
	score_time_t scoreTime = timeWarper_.WarpTimestamp(now);
	TempoPoint tempoNow = tempoMap_.GetTempoAt(scoreTime);

	tempo_t conductedTempo = BeatLengthEstimate();

	return static_cast<speed_t>(tempoNow.tempo().count()) / conductedTempo.count();
}

speed_t
TempoFollower::SpeedFromBeatCatchup() const
{
	TempoPoint const lastBeatPoint = LastBeat();

	// TODO
	return speed_;
}

TempoPoint
TempoFollower::LastBeat() const
{
	real_time_t lastBeatRt = beatHistory_.AllEvents().LastTimestamp();
	score_time_t lastBeatSt = timeWarper_.WarpTimestamp(lastBeatRt);
	return tempoMap_.GetTempoAt(lastBeatSt);
}

tempo_t
TempoFollower::BeatLengthEstimate() const
{
	boost::array<double, 4> weights = { 0.4, 0.3, 0.2, 0.1 };

	auto beats = beatHistory_.AllEvents().timestampRange() | boost::adaptors::reversed;
	assert(beats.size() > 4);

	double weightedSum = 0.0;
	auto bIt = beats.begin();
	real_time_t laterBeat = *bIt;
	++bIt;
	for(auto wIt = weights.begin(); wIt != weights.end(); ++wIt, ++bIt) {
		weightedSum += *wIt * (laterBeat - *bIt).count();
		laterBeat = *bIt;
	}

	// Cast down to whole units (micro, nano, whatever, don't care about rounding...)
	duration_t estimate(static_cast<duration_t::rep>(weightedSum));
	return time::duration_cast<tempo_t>(estimate);
}

} // namespace ScoreFollower
} // namespace cf
