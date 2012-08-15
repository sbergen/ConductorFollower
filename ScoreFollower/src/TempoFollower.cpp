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
	//LOG("Classification quality: %1%", classification.Quality());
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
		acceleration_.SetParameters(speed_, beatTime, 0.0 * score::fractions_per_second, 0.0 * score::seconds); 

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

		auto accelerationTime = time_quantity(1.0 * score::seconds);
		auto offsetEstimate = BeatOffsetEstimate();
		auto tempoChange = offsetEstimate / boost::units::pow<2>(accelerationTime);
		SpeedFunction::SpeedChangeRate acceleration(tempoChange / tempoNow.tempo());

		LOG("Beat offset: %1%, tempo change: %2%, acceleartion: %3%",
			offsetEstimate, tempoChange.value(), acceleration.value());

		acceleration_.SetParameters(speed_, time, acceleration, accelerationTime);
	}

	speed_ = acceleration_.NewSpeed(time);
	return speed_;
}

BeatClassification
TempoFollower::ClassifyBeatAt(real_time_t const & time, double prob)
{
	// Special case for the first two beats, TODO make better
	if (beatHistory_.AllEvents().Size() < 2) {
		return BeatClassification::PreparatoryBeat(time);
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
	auto offset = BeatOffsetEstimate(latestBeat);
	double speedChange = std::abs(offset / (1.0 * score::quarter_notes));
	double diffFromUnity = std::abs(1.0 - (speed_ + speedChange));

	//double quality = 7.0 - 5.0 * speedChange - 1.0 * diffFromUnity;
	double quality = 7.0 - 10.0 * speedChange;
	return quality;
}

double
TempoFollower::ClassificationSelector(BeatClassification const & latestBeat, double quality) const
{
	return quality;

	/*
	double bonusFromPrevious = 0.0;

	if (!beatHistory_.AllEvents().Empty()) {
		bonusFromPrevious = beatHistory_.AllEvents().Back().data.QualityAs(latestBeat.classification());
	}

	double bonus = std::max(0.0, std::min(quality, bonusFromPrevious));
	double result = quality + 1.5 * bonus;
	return result;
	*/
}

beat_pos_t
TempoFollower::BeatOffsetEstimate() const
{
	auto lastBeat = beatHistory_.AllEvents().Back().data;
	return BeatOffsetEstimate(lastBeat);
}

beat_pos_t
TempoFollower::BeatOffsetEstimate(BeatClassification const & latestBeat) const
{
	return latestBeat.offset();
}

} // namespace ScoreFollower
} // namespace cf
