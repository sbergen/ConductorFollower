#include "TempoFollower.h"

#include "globals.h"

namespace cf {
namespace ScoreFollower {

TempoFollower::TempoFollower()
	: tempoMap_()
	, beatHistory_(100) // Arbitrary length, should be long enough...
	, newBeats_(false)
	, speed_(1.0)
{
}

void
TempoFollower::RegisterBeat(score_time_t const & beatTime)
{
	assert(beatTime > beatHistory_.AllEvents().LastTimestamp());

	// Beats will probably include a probability later...
	beatHistory_.RegisterEvent(beatTime, 1.0);
	newBeats_ = true;
}


speed_t
TempoFollower::SpeedEstimateAt(score_time_t const & time)
{
	if (!newBeats_) { return speed_; }
	newBeats_ = false;

	// Ignore beginning for now...
	if (beatHistory_.AllEvents().Size() < 16) { return speed_; }

	TempoMap::TempoPoint tempoNow = tempoMap_.GetTempoAt(time);
	score_time_t avgConductedTempo = AvgConductedBeatDuration(2);

	speed_ *= (double)tempoNow.tempo.count() / (double)avgConductedTempo.count();

	return speed_;
}

score_time_t
TempoFollower::AvgConductedBeatDuration(int averageOver)
{
	auto lastBeats = beatHistory_.LastNumEvnets(averageOver + 1);
	score_time_t duration = lastBeats.LastTimestamp() - lastBeats.timestamp();
	return duration / averageOver;
}

} // namespace ScoreFollower
} // namespace cf
