#include "TempoFollower.h"

#include <boost/range/adaptors.hpp>

#include "ScoreFollower/Follower-private.h"

#include "BeatClassifier.h"
#include "globals.h"
#include "TimeWarper.h"

namespace cf {
namespace ScoreFollower {

TempoFollower::TempoFollower(TimeWarper const & timeWarper, FollowerPrivate & parent)
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
	beatHistory_.RegisterEvent(beatTime, classification);
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

	speed_t tempoSpeed = SpeedFromConductedTempo(tempoNow, time);
	speed_t phaseSpeed = SpeedFromBeatCatchup(tempoNow, 2.0);

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

	auto & status = parent_.status();
	status.SetValue<Status::SpeedFromTempo>(tempoSpeed);
	status.SetValue<Status::SpeedFromPhase>(phaseSpeed);

	return speed_;
}

TempoFollower::BeatClassification
TempoFollower::ClassifyBeatAt(real_time_t const & time)
{
	if (beatHistory_.AllEvents().Size() < 1) { return BeatClassification(0, 1.0); }

	score_time_t prevBeatScoreTime = timeWarper_.WarpTimestamp(beatHistory_.AllEvents().LastTimestamp());
	TempoPoint prevTempoPoint = tempoMap_.GetTempoAt(prevBeatScoreTime);

	score_time_t beatScoreTime = timeWarper_.WarpTimestamp(time);
	TempoPoint tempoPoint = tempoMap_.GetTempoAt(beatScoreTime);
		
	beat_pos_t rawOffset = tempoPoint.position() - prevTempoPoint.position();
	auto classification = BeatClassifier::ClassifyBeat(rawOffset);
	beat_pos_t offset = rawOffset - (static_cast<beat_pos_t>(classification.eightsSincePrevious) / 2);
	LOG("rawOffset: %1%, classification.eightsSincePrevious: %2%, offset: %3%",
		rawOffset, classification.eightsSincePrevious, offset);
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
	score_time_t catchup = time::multiply(tempoNow.tempo(), BeatOffsetEstimate());
	tempo_t newTempo = tempoNow.tempo() + time::divide(catchup, catchupTime);
	speed_t speed = static_cast<speed_t>(tempoNow.tempo().count()) / newTempo.count();
	
	assert(speed >= 0.0);
	return speed;
}

beat_pos_t
TempoFollower::BeatOffsetEstimate() const
{
	assert(beatHistory_.AllEvents().Size() > 4);

	auto times = beatHistory_.AllEvents().timestampRange() | boost::adaptors::reversed;
	auto classifications = beatHistory_.AllEvents().dataRange() | boost::adaptors::reversed;

	boost::array<double, 4> weights = { 10.0, 6.0, 2.0, 1.0 };
	beat_pos_t weightedSum = 0.0;

	auto tIt = times.begin();
	auto cIt = classifications.begin();
	for(auto wIt = weights.begin(); wIt != weights.end(); ++wIt, ++tIt, ++cIt) {
		// weight by probablitiy
		*wIt *= cIt->probability;
		weightedSum += *wIt * cIt->offset;
	}

	double normalizationTerm = std::accumulate(weights.begin(), weights.end(), 0.0);
	double estimate = weightedSum / normalizationTerm;
	return estimate;
}

tempo_t
TempoFollower::BeatLengthEstimate() const
{
	boost::array<double, 4> weights = { 0.5, 0.25, 0.15, 0.1 };

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
}

} // namespace ScoreFollower
} // namespace cf
