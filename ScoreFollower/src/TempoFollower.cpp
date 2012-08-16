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
	auto start = tempoMap_.GetScorePositionAt(0.0 * score::seconds);
	startTempoEstimator_.SetStartTempo(start.tempo());
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
	if (true /* TODO rejection! */) {
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
		tempo_t tempoNow = tempoMap_.GetScorePositionAt(scoreTime).tempo();

		// TODO Move to estimator
		auto accelerationTime = time_quantity(0.8 * score::seconds);
		auto offsetEstimate = -BeatOffsetEstimate();
		auto tempoChange = offsetEstimate / boost::units::pow<2>(accelerationTime);
		SpeedFunction::SpeedChangeRate acceleration(tempoChange / tempoNow);

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
	ScorePosition tempoPoint = tempoMap_.GetScorePositionAt(beatScoreTime);

	LOG("Got beat at: %1%", tempoPoint.position());

	BeatClassification absoluteClassification(time, clarity, tempoPoint.position());
	BeatClassification relativeClassification(absoluteClassification);

	if (!beatHistory_.AllEvents().Empty()) {
		BeatClassification const & previousBeat = beatHistory_.AllEvents().Back().data;
		relativeClassification.UpdatedEstimate(tempoPoint.position() - previousBeat.beatPosition());
	}

	if (absoluteClassification.quality() >= relativeClassification.quality()) {
		LOG("Using absolute classification, offset: %1%", absoluteClassification.MostLikelyOffset());
		return absoluteClassification;
	} else {
		LOG("Using relative classification, offset: %1%", relativeClassification.MostLikelyOffset());
		return relativeClassification;
	}
}

beat_pos_t
TempoFollower::BeatOffsetEstimate() const
{
	auto lastBeat = beatHistory_.AllEvents().Back().data;
	return lastBeat.MostLikelyOffset();
}

} // namespace ScoreFollower
} // namespace cf
