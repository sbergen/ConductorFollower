#include "TempoFollower.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerStatus.h"

#include "BeatClassifier.h"
#include "globals.h"
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
		tempo_t tempo = tempoMap_.GetTempoAt(score_time_t::zero()).tempo();
		auto beats = beatHistory_.AllEvents();
		real_time_t firstBeat = beats[0].timestamp;
		real_time_t secondBeat = beats[1].timestamp;
		tempo_t cTempo = time::duration_cast<tempo_t>(secondBeat - firstBeat);
		speed_ = static_cast<speed_t>(tempo.count()) / cTempo.count();

		LOG("Starting tempo: %1%, (%3% - %4%) speed_: %2%", cTempo, speed_, secondBeat, firstBeat);
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

	speed_t tempoSpeed = SpeedFromConductedTempo(tempoNow, time);
	speed_t phaseSpeed = SpeedFromBeatCatchup(tempoNow, 1.5);

	speed_ = phaseSpeed;
	
	/*
	auto & options = parent_.options();
	double phaseThresh;
	options.GetValue<Options::TempoFromPhaseThresh>(phaseThresh);
	double tempoDiff = std::abs((phaseSpeed / tempoSpeed) - 1.0) * 100;
	
	if (tempoDiff < phaseThresh) {
		speed_ = phaseSpeed;	
	} else {
		speed_ = tempoSpeed;
	}
	*/

	auto status = parent_.status().write();
	status->SetValue<Status::SpeedFromTempo>(tempoSpeed);
	status->SetValue<Status::SpeedFromPhase>(phaseSpeed);

	return speed_;
}

TempoFollower::BeatClassification
TempoFollower::ClassifyBeatAt(real_time_t const & time)
{
	// Special case for the first two beats
	if (beatHistory_.AllEvents().Size() == 0) { return BeatClassification(-1, 1.0); }
	if (beatHistory_.AllEvents().Size() == 1) { return BeatClassification(2, 1.0); }

	score_time_t prevBeatScoreTime = timeWarper_.WarpTimestamp(beatHistory_.AllEvents().LastTimestamp());
	TempoPoint prevTempoPoint = tempoMap_.GetTempoAt(prevBeatScoreTime);

	score_time_t beatScoreTime = timeWarper_.WarpTimestamp(time);
	TempoPoint tempoPoint = tempoMap_.GetTempoAt(beatScoreTime);
	
	beat_pos_t rawOffset = tempoPoint.position() - prevTempoPoint.position();
	auto classification = BeatClassifier::ClassifyBeat(rawOffset);
	beat_pos_t offset = rawOffset - (static_cast<beat_pos_t>(classification.eightsSincePrevious) / 2);

	// Remove beats that happen "too soon"
	if (classification.eightsSincePrevious == 1 && classification.probability < 0.1) {
		LOG("Dropping beat at %1%, prob: %2%", time, classification.probability);
		classification.probability = 0.0;
	}

	return BeatClassification(offset, classification.probability);
}

speed_t
TempoFollower::SpeedFromConductedTempo(TempoPoint const & tempoNow, real_time_t const & now) const
{
	tempo_t conductedTempo = BeatLengthEstimate();
	return static_cast<speed_t>(tempoNow.tempo().count()) / conductedTempo.count();
}

speed_t
TempoFollower::SpeedFromBeatCatchup(TempoPoint const & tempoNow, beat_pos_t catchupTime) const
{
	tempo_t actualTempo = time::divide(tempoNow.tempo(), speed_);
	score_time_t catchup = time::multiply(actualTempo, BeatOffsetEstimate());
	tempo_t newTempo = actualTempo + time::divide(catchup, catchupTime);
	speed_t speed = static_cast<speed_t>(tempoNow.tempo().count()) / newTempo.count();

	assert(speed >= 0.0);
	return speed;
}

beat_pos_t
TempoFollower::BeatOffsetEstimate() const
{
	boost::array<double, 4> weights = { 10.0, 6.0, 2.0, 1.0 };
	beat_pos_t weightedSum = 0.0;
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

	double estimate = weightedSum / normalizationTerm;
	return estimate;
}

tempo_t
TempoFollower::BeatLengthEstimate() const
{
	// TODO implement if needed!
/*	boost::array<double, 4> weights = { 0.5, 0.25, 0.15, 0.1 };

	auto beats = beatHistory_.AllEvents().timestampRange() | boost::adaptors::reversed;
	assert(beats.size() > 4);

	duration_t weightedSum = duration_t::zero();
	auto bIt = beats.begin();
	real_time_t laterBeat = *bIt;
	++bIt;
	for(auto wIt = weights.begin(); wIt != weights.end(); ++wIt, ++bIt) {
		weightedSum += time::multiply(laterBeat - *bIt, *wIt);
		laterBeat = *bIt;
	}

	// Cast down to whole units (micro, nano, whatever, don't care about rounding...)
	return time::duration_cast<tempo_t>(weightedSum);
	*/
	return tempo_t(10000);
}

} // namespace ScoreFollower
} // namespace cf
