#include "TempoFollower.h"

namespace cf {
namespace ScoreFollower {

TempoFollower::TempoFollower()
	: tempoMap_()
	, beatHistory_(100) // Arbitrary length, should be long enough...
{
}

void
TempoFollower::RegisterBeat(score_time_t const & beatTime)
{
	assert(beatTime > beatHistory_.AllEvents().LastTimestamp());

	// Beats will probably include a probability later...
	beatHistory_.RegisterEvent(beatTime, 1.0);
}


speed_t
TempoFollower::SpeedEstimateAt(score_time_t const & time)
{
	// Ignore beginning for now...
	if (beatHistory_.AllEvents().Size() < 16) { return 1.0; }
	
	speed_t speed = 1.0;
	
	TempoMap::TempoPoint tp = tempoMap_.GetTempoAt(time);
	auto lastMeasure = beatHistory_.LastNumEvnets(3);

	real_time_t::duration lmDuration = lastMeasure.timestampRange().back() - lastMeasure.timestampRange().front();
	tempo_t cTempo = time::duration_cast<tempo_t>(lmDuration / 2);

	// tempos are durations, so the division is this way around...
	speed = (double) tp.tempo.count() / (double)cTempo.count();

	return speed;
}


} // namespace ScoreFollower
} // namespace cf
