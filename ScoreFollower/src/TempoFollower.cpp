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
TempoFollower::RegisterBeat(real_time_t const & beatTime, double prob)
{
	assert(beatTime > beatHistory_.AllEvents().LastTimestamp());


	auto classification = ClassifyBeatAt(beatTime, prob);
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
TempoFollower::ClassifyBeatAt(real_time_t const & time, double prob)
{
	// Special case for the first two beats
	if (beatHistory_.AllEvents().Size() == 0) { return BeatClassification(-1 * score::beats, 1.0); }
	if (beatHistory_.AllEvents().Size() == 1) { return BeatClassification(2 * score::beats, 1.0); }


	/*************************/

	score_time_t prevBeatScoreTime = timeWarper_.WarpTimestamp(beatHistory_.AllEvents().LastTimestamp());
	TempoPoint prevTempoPoint = tempoMap_.GetTempoAt(prevBeatScoreTime);

	score_time_t beatScoreTime = timeWarper_.WarpTimestamp(time);
	TempoPoint tempoPoint = tempoMap_.GetTempoAt(beatScoreTime);
	
	beat_pos_t rawOffset = tempoPoint.position() - prevTempoPoint.position();

	BeatClassification bestClassification(-1 * score::quarter_notes, -1.0);
	double leastChange = 100.0;

	// TODO add eights
	for (int i = 1; i < 16; ++i) {
		auto offset = rawOffset - (i * score::quarter_notes);
		BeatClassification classification(offset, prob);
		auto offsetEst = BeatOffsetHypothesis(classification);
		double speedChange = std::abs(offsetEst / (1.0 * score::quarter_notes));

		if (speedChange < leastChange) {
			leastChange = speedChange;
			bestClassification = classification;
		}
	}


	assert(bestClassification.probability > 0.0);
	LOG("Beat classified as: %1%, with prob: %2%", bestClassification.offset.value(), bestClassification.probability);
	return bestClassification;

	/*************************/

	/*
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
	*/
}


speed_t
TempoFollower::SpeedFromBeatCatchup(TempoPoint const & tempoNow, beat_pos_t catchupTime) const
{	
	// TODO wtf? After cleaning this up this doesn't look right at all...
	//speed_t oldSpeed = speed_ / (1.0 + (BeatOffsetEstimate() / catchupTime));

	auto bos = BeatOffsetEstimate();
	speed_t speed = speed_ - (bos / catchupTime);

	assert(speed >= 0.0);
	return speed;
}

beat_pos_t
TempoFollower::BeatOffsetEstimate() const
{
	auto events = beatHistory_.AllEvents();
	auto lastBeat = events.PopBack().data;
	return BeatOffsetHypothesis(lastBeat, events);
}

beat_pos_t
TempoFollower::BeatOffsetHypothesis(BeatClassification const & latestBeat) const
{
	return BeatOffsetHypothesis(latestBeat, beatHistory_.AllEvents());
}

beat_pos_t
TempoFollower::BeatOffsetHypothesis(BeatClassification const & latestBeat,
		BeatHistoryBuffer::Range const & otherBeats) const
{
	double const firstWeight = 10.0 * latestBeat.probability;
	boost::array<double, 4> weights = { 6.0, 2.0, 1.0 };
	
	beat_pos_t weightedSum = firstWeight * latestBeat.offset;
	double normalizationTerm = firstWeight;

	auto wIt = weights.begin();
	auto wEnd = weights.end();
	otherBeats.ReverseForEachWhile(
		[&weightedSum, &normalizationTerm, &wIt, wEnd] (real_time_t const & time, BeatClassification const & classification) -> bool
	{
		double weight = *wIt * classification.probability;
		weightedSum += weight * classification.offset;
		normalizationTerm += weight;
		return ++wIt == wEnd;
	});

	beat_pos_t estimate = weightedSum / normalizationTerm;
	return estimate;
}


} // namespace ScoreFollower
} // namespace cf
