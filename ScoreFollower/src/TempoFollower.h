#pragma once

#include <vector>

#include <boost/utility.hpp>

#include "cf/EventBuffer.h"
#include "cf/units_math.h"

#include "MotionTracker/StartGestureData.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/StatusEvents.h"

#include "Data/Score.h"
#include "Data/ScoreEvent.h"

#include "TempoMap.h"
#include "BeatClassification.h"
#include "TempoFunction.h"
#include "StartTempoEstimator.h"
#include "BeatClassifier.h"
#include "Fermata.h"

namespace cf {
namespace ScoreFollower {

class TimeWarper;
class Follower;

class TempoFollower : public boost::noncopyable
{
public:
	TempoFollower(TimeWarper const & timeWarper, Follower & parent);

	void ReadScore(ScoreReader & reader);
	void LearnPatterns(Data::PatternMap const & patterns) { beatClassifier_.LearnPatterns(patterns); }
	void LearnScoreEvents(Data::ScoreEventList const & events);

	void RegisterStartGesture(MotionTracker::StartGestureData const & data);
	BeatEvent RegisterBeat(real_time_t const & beatTime, double prob);

	real_time_t StartTimeEstimate() { return startTempoEstimator_.StartTimeEstimate(); }
	speed_t SpeedEstimateAt(real_time_t const & time);

	ScorePosition ScorePositionAt(real_time_t const & time) const;

private:
	class ScoreEventBuilder;
	typedef EventBuffer<Data::TempoSensitivityChange, score_time_t, std::vector> TempoSensitivityBuffer;
	typedef EventBuffer<Fermata, score_time_t, std::vector> FermataBuffer;

private:
	BeatClassification ClassifyBeatAt(real_time_t const & time, double clarity);

	score_time_t AccelerationTimeAt(score_time_t time);

	void EnterFermata(real_time_t const & realTime, score_time_t const & scoreTime);
	void LookupNextFermata(score_time_t const & timeNow);

private: // Basic tempo following
	TimeWarper const & timeWarper_;
	Follower & parent_;
	TempoMap tempoMap_;

	StartTempoEstimator startTempoEstimator_;
	BeatClassifier beatClassifier_;
	TempoFunction tempoFunction_;

private: // Score events
	TempoSensitivityBuffer tempoSensitivities_;
	FermataBuffer fermatas_;

	FermataState nextFermata_;
	tempo_t fermataReferenceTempo_;
	real_time_t fermataEndTime_;
};

} // namespace ScoreFollower
} // namespace cf
