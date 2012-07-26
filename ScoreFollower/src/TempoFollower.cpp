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
	if (classification.classification().value() > 0) {
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
		speed_ = 1.0; // temporary override
	}
}


speed_t
TempoFollower::SpeedEstimateAt(real_time_t const & time)
{
	score_time_t scoreTime = timeWarper_.WarpTimestamp(time);

	if (newBeats_) {
		newBeats_ = false;

		// The first few beats are not useful
		if (beatHistory_.AllEvents().Size() <= 3) { return speed_; }

		score_time_t scoreTime = timeWarper_.WarpTimestamp(time);
		TempoPoint tempoNow = tempoMap_.GetTempoAt(scoreTime);

		auto catchupTime = 1.0 * score::beats;
		targetSpeed_ = SpeedFromBeatCatchup(tempoNow, catchupTime);
		if (targetSpeed_ > 2.0) { targetSpeed_ = 2.0; } // TODO limit better

		score_time_t accelerationTime(catchupTime / tempoNow.tempo());
		accelerateUntil_ = timeWarper_.InverseWarpTimestamp(scoreTime + accelerationTime);
		auto accelerationPerTimeUnit = (targetSpeed_ - speed_) / time::quantity_cast<time_quantity>(accelerateUntil_ - time);

		auto speed = speed_;
		acceleration_ = [speed, accelerationPerTimeUnit, time] (real_time_t const & newTime) ->
		double
		{
			auto timeDiff = time::quantity_cast<time_quantity>(newTime - time);
			return speed + (timeDiff * accelerationPerTimeUnit);
		};
	}

	if (time < accelerateUntil_) {
		speed_ = acceleration_(time);
		//LOG("Accelerated to: %1% (%2%), towards: %3% (%4%)", speed_, time, targetSpeed_, accelerateUntil_);
	}

	auto status = parent_.status().write();
	status->SetValue<Status::SpeedFromPhase>(speed_);

	return speed_;
}

BeatClassification
TempoFollower::ClassifyBeatAt(real_time_t const & time, double prob)
{
	// Special case for the first two beats, TODO make better
	if (beatHistory_.AllEvents().Size() == 0) {
		return BeatClassification(time, 1.0 * score::beats, 1.0);
	}

	if (beatHistory_.AllEvents().Size() == 1) {
		return BeatClassification(time, 1.0 * score::beats, 1.0 /*???*/);
	}


	/*************************/

	score_time_t prevBeatScoreTime = timeWarper_.WarpTimestamp(beatHistory_.AllEvents().LastTimestamp());
	TempoPoint prevTempoPoint = tempoMap_.GetTempoAt(prevBeatScoreTime);

	score_time_t beatScoreTime = timeWarper_.WarpTimestamp(time);
	TempoPoint tempoPoint = tempoMap_.GetTempoAt(beatScoreTime);
	
	beat_pos_t rawOffset = tempoPoint.position() - prevTempoPoint.position();

	BeatClassification classification(time, rawOffset, prob);

	LOG("---- Classifying beat at raw offset: %1% -----", rawOffset);

	classification.Evaluate(
		boost::bind(&TempoFollower::ClassificationQuality, this, _1),
		boost::bind(&TempoFollower::ClassificationSelector, this, _1, _2));

	/*
	assert(bestClassification.probability > 0.0);
	LOG("Beat classified as: %1%, with prob: %2%", bestClassification.offset.value(), bestClassification.probability);
	*/
	
	return classification;

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

double
TempoFollower::ClassificationQuality(BeatClassification const & latestBeat) const
{
	auto offset = BeatOffsetHypothesis(latestBeat);
	double speedChange = offset / (1.0 * score::quarter_notes);
	double diffFromUnity = std::abs(1.0 - (speed_ + speedChange));

	double quality = 7.0 - 5.0 * std::abs(speedChange) - diffFromUnity;
	LOG("Classification result as %1%: offset: %2%, speedChange: %3%, diffFromUnity: %4%",
		latestBeat.classification(), offset, speedChange, diffFromUnity);
	return quality;
}

double
TempoFollower::ClassificationSelector(BeatClassification const & latestBeat, double quality) const
{
	double bonusFromPrevious = 0.0;

	if (!beatHistory_.AllEvents().Empty()) {
		bonusFromPrevious = beatHistory_.AllEvents().Back().data.QualityAs(latestBeat.classification());
	}

	double bonus = std::max(0.0, std::min(quality, bonusFromPrevious));
	double result = quality + 1.5 * bonus;
	LOG("Classification quality as %1%: quality: %2% + bonusFromPrev: %3%, result: %4%",
		latestBeat.classification(), quality, bonusFromPrevious, result);
	return result;
}


speed_t
TempoFollower::SpeedFromBeatCatchup(TempoPoint const & tempoNow, beat_pos_t catchupTime) const
{
	auto bos = BeatOffsetEstimate();
	speed_t speed = speed_ + (bos / catchupTime);

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
	double const firstWeight = 10.0 * latestBeat.clarity();
	boost::array<double, 4> weights = { 3.0, 2.0, 1.0 };
	
	beat_pos_t weightedSum = firstWeight * latestBeat.offset();
	double normalizationTerm = firstWeight;

	auto wIt = weights.begin();
	auto wEnd = weights.end();
	otherBeats.ReverseForEachWhile(
		[&weightedSum, &normalizationTerm, &wIt, wEnd] (real_time_t const & time, BeatClassification const & classification) -> bool
	{
		double weight = *wIt * classification.clarity();
		weightedSum += weight * classification.offset();
		normalizationTerm += weight;
		return ++wIt == wEnd;
	});

	beat_pos_t estimate = weightedSum / normalizationTerm;
	return estimate;
}


} // namespace ScoreFollower
} // namespace cf
