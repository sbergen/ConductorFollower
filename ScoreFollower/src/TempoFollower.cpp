#include "TempoFollower.h"

#include "cf/globals.h"
#include "cf/math.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerStatus.h"

#include "TimeWarper.h"

namespace cf {
namespace ScoreFollower {

namespace si = boost::units::si;

TempoFollower::TempoFollower(TimeWarper const & timeWarper, Follower & parent)
	: timeWarper_(timeWarper)
	, parent_(parent)
	, tempoMap_()
	, startTempoEstimator_()
	, beatHistory_(100) // Arbitrary length, should be long enough...
	, newBeats_(false)
{
}

void
TempoFollower::ReadScore(ScoreReader & reader)
{
	tempoMap_.ReadScore(reader);
	auto startTempo = tempoMap_.GetTempoAt(0.0 * score::seconds);
	startTempoEstimator_.SetStartTempo(startTempo.tempo());
}

void
TempoFollower::RegisterPreparatoryBeat(real_time_t const & time)
{
	startTempoEstimator_.RegisterPreparatoryBeat(time);
}

void
TempoFollower::RegisterBeat(real_time_t const & beatTime, double clarity)
{
	assert(beatTime > beatHistory_.AllEvents().LastTimestamp());

	if (!startTempoEstimator_.Done()) {
		auto speed = startTempoEstimator_.SpeedFromBeat(beatTime, clarity);
		acceleration_.SetConstantSpeed(speed);
		return;
	}

	auto classification = ClassifyBeatAt(beatTime, clarity);
	if (/*bu::abs(classification.MostLikelyOffset()) < 0.25 * score::quarter_notes*/
		true) {
		beatHistory_.RegisterEvent(beatTime, classification);
		newBeats_ = true;
	}
}


speed_t
TempoFollower::SpeedEstimateAt(real_time_t const & time)
{
	if (newBeats_) {
		newBeats_ = false;

		score_time_t scoreTime = timeWarper_.WarpTimestamp(time);
		TempoPoint tempoNow = tempoMap_.GetTempoAt(scoreTime);

		// TODO Move to estimator
		auto accelerationTime = time_quantity(1.0 * score::seconds);
		auto offsetEstimate = BeatOffsetEstimate();
		auto tempoChange = offsetEstimate / boost::units::pow<2>(accelerationTime);
		SpeedFunction::SpeedChangeRate acceleration(tempoChange / tempoNow.tempo());

		LOG("Beat offset: %1%, tempo change: %2%, acceleartion: %3%",
			offsetEstimate, tempoChange.value(), acceleration.value());
		
		auto speed = acceleration_.SpeedAt(time);
		acceleration_.SetParameters(speed, time, acceleration, accelerationTime);
	}

	return acceleration_.SpeedAt(time);
}

BeatClassification
TempoFollower::ClassifyBeatAt(real_time_t const & time, double clarity)
{
	// TODO allow different estimation modes
	score_time_t beatScoreTime = timeWarper_.WarpTimestamp(time);
	TempoPoint tempoPoint = tempoMap_.GetTempoAt(beatScoreTime);
	
	LOG("Got beat at: %1%", tempoPoint.position());

	return BeatClassification(time, clarity, tempoPoint.position());
}

beat_pos_t
TempoFollower::BeatOffsetEstimate() const
{
	auto lastBeat = beatHistory_.AllEvents().Back().data;
	return lastBeat.MostLikelyOffset();
}

} // namespace ScoreFollower
} // namespace cf
