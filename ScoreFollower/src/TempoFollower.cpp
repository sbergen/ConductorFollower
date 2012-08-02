#include "TempoFollower.h"

#include "cf/globals.h"
#include "cf/math.h"

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
TempoFollower::RegisterPreparatoryBeat(real_time_t const & time)
{
	beatHistory_.RegisterEvent(time, BeatClassification::PreparatoryBeat(time));
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

		auto catchupTime = 2.0 * score::beats;
		targetSpeed_ = SpeedFromBeatCatchup(tempoNow, catchupTime);
		if (targetSpeed_ > 2.0) { targetSpeed_ = 2.0; } // TODO limit better

		accelerateUntil_ = time + boost::chrono::duration_cast<duration_t>(seconds_t(0.5));
		auto accelerationPerTimeUnit = (targetSpeed_ - speed_) / time::quantity_cast<time_quantity>(accelerateUntil_ - time);

		// TODO allocates!
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
	}

	auto status = parent_.status().writer();
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
		[this](BeatClassification const & latestBeat)
		{
			return ClassificationQuality(latestBeat);
		},
		[this](BeatClassification const & latestBeat, double quality)
		{
			return ClassificationSelector(latestBeat, quality);
		});
	
	return classification;
}

double
TempoFollower::ClassificationQuality(BeatClassification const & latestBeat) const
{
	auto offset = BeatOffsetHypothesis(latestBeat);
	double speedChange = std::abs(offset / (1.0 * score::quarter_notes));
	double diffFromUnity = std::abs(1.0 - (speed_ + speedChange));

	double quality = 7.0 - 5.0 * speedChange - 1.0 * diffFromUnity;
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
	boost::array<double, 3> weights = { 5.0, 2.0, 1.0 };
	
	beat_pos_t weightedSum = firstWeight * latestBeat.offset();
	double normalizationTerm = firstWeight;

	auto wIt = weights.begin();
	auto wEnd = weights.end();
	otherBeats.ReverseForEachWhile(
		[&, wEnd] (real_time_t const & time, BeatClassification const & classification) -> bool
	{
		auto offset = classification.offset().value();
		offset = math::sgn(offset) * 2 * std::pow(offset, 2);

		double weight = *wIt * classification.clarity();
		weightedSum += weight * beat_pos_t::from_value(offset);
		normalizationTerm += weight;
		return ++wIt == wEnd;
	});

	beat_pos_t estimate = weightedSum / normalizationTerm;
	return estimate;
}


} // namespace ScoreFollower
} // namespace cf
