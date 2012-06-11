#include "TempoFollower.h"

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

	TempoPoint const lastBeatPoint = LastBeat();

	return speed_;
}


TempoPoint
TempoFollower::LastBeat()
{
	real_time_t lastBeatRt = beatHistory_.AllEvents().LastTimestamp();
	score_time_t lastBeatSt = timeWarper_.WarpTimestamp(lastBeatRt);
	return tempoMap_.GetTempoAt(lastBeatSt);
}

} // namespace ScoreFollower
} // namespace cf
