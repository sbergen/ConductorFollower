#include "TempoFollower.h"

#include "cf/globals.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerStatus.h"

#include "TimeWarper.h"

namespace cf {
namespace ScoreFollower {

TempoFollower::TempoFollower(TimeWarper const & timeWarper, Follower & parent)
	: timeWarper_(timeWarper)
	, parent_(parent)
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

	auto classification = ClassifyBeatAt(beatTime);
	if (classification.probability > 0) {
		beatHistory_.RegisterEvent(beatTime, classification);
		newBeats_ = true;
	}

	// Beginning (TODO, clean up)
	if (beatHistory_.AllEvents().Size() == 2) {
		tempo_t tempo = tempoMap_.GetTempoAt(0.0 * score::seconds).tempo();
		auto beats = beatHistory_.AllEvents();
		real_time_t firstBeat = beats[0].timestamp;
		real_time_t secondBeat = beats[1].timestamp;
		time_quantity beatDiff = time::quantity_cast<time_quantity>(secondBeat - firstBeat);
		tempo_t conductedTempo(1.0 * score::beat / beatDiff);
		speed_ = conductedTempo / tempo;

		LOG("Starting tempo: %1%, (%3% - %4%) speed_: %2%", conductedTempo, speed_, secondBeat, firstBeat);
	}
}


speed_t
TempoFollower::SpeedEstimateAt(real_time_t const & time)
{
	if (!newBeats_) { return speed_; }
	newBeats_ = false;

	// The first few beats are not useful
	if (beatHistory_.AllEvents().Size() <= 3) { return speed_; }

	score_time_t scoreTime = timeWarper_.WarpTimestamp(time);
	TempoPoint tempoNow = tempoMap_.GetTempoAt(scoreTime);

	speed_ = SpeedFromBeatCatchup(tempoNow, 1.5 * score::beats);

	// TODO limit better
	if (speed_ > 2.0) { speed_ = 2.0; }

	auto status = parent_.status().write();
	status->SetValue<Status::SpeedFromPhase>(speed_);

	return speed_;
}

TempoFollower::BeatClassification
TempoFollower::ClassifyBeatAt(real_time_t const & time)
{
	// Special case for the first two beats
	if (beatHistory_.AllEvents().Size() == 0) { return BeatClassification(-1 * score::beats, 1.0); }
	if (beatHistory_.AllEvents().Size() == 1) { return BeatClassification(2 * score::beats, 1.0); }

	score_time_t prevBeatScoreTime = timeWarper_.WarpTimestamp(beatHistory_.AllEvents().LastTimestamp());
	TempoPoint prevTempoPoint = tempoMap_.GetTempoAt(prevBeatScoreTime);

	score_time_t beatScoreTime = timeWarper_.WarpTimestamp(time);
	TempoPoint tempoPoint = tempoMap_.GetTempoAt(beatScoreTime);
	
	beat_pos_t rawOffset = tempoPoint.position() - prevTempoPoint.position();
	auto classification = beatClassifier_.ClassifyBeat(rawOffset);
	beat_pos_t offset = rawOffset - classification.beatsSincePrevious;

	// Remove beats that happen "too soon"
	if ((classification.beatsSincePrevious < beats_t(1.0  * score::eight_note)) &&
		classification.probability < 0.1)
	{
		LOG("Dropping beat at %1%, prob: %2%", time, classification.probability);
		classification.probability = 0.0;
	}

	return BeatClassification(offset, classification.probability);
}


speed_t
TempoFollower::SpeedFromBeatCatchup(TempoPoint const & tempoNow, beat_pos_t catchupTime) const
{
	beats_t stretchedLength = catchupTime + BeatOffsetEstimate();
	speed_t speed = stretchedLength / catchupTime;

	/*
	tempo_t actualTempo = tempoNow.tempo() * speed_;
	score_time_t catchup(BeatOffsetEstimate() / actualTempo);
	score_time_t lengthOfTimespan = (catchupTime / actualTempo) + catchup;
	tempo_t newTempo(catchupTime / lengthOfTimespan);
	speed_t speed = newTempo / tempoNow.tempo();
	*/

	assert(speed >= 0.0);
	return speed;
}

beat_pos_t
TempoFollower::BeatOffsetEstimate() const
{
	boost::array<double, 4> weights = { 10.0, 6.0, 2.0, 1.0 };
	beat_pos_t weightedSum = 0.0 * score::beats;
	double normalizationTerm = 0.0;

	auto wIt = weights.begin();
	auto wEnd = weights.end();
	beatHistory_.AllEvents().ReverseForEachWhile(
		[&weightedSum, &normalizationTerm, &wIt, wEnd] (real_time_t const & time, BeatClassification const & classification) -> bool
	{
		double weight = *wIt;
		weightedSum += weight * classification.offset;
		normalizationTerm += weight;
		return ++wIt == wEnd;
	});

	beat_pos_t estimate = weightedSum / normalizationTerm;
	return estimate;
}

} // namespace ScoreFollower
} // namespace cf
