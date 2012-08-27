#pragma once

#include <vector>

#include "cf/EventBuffer.h"
#include "cf/units_math.h"

#include "MotionTracker/StartGestureData.h"
#include "Data/ScoreEvent.h"

#include "ScoreFollower/types.h"
#include "Data/Score.h"

#include "TempoMap.h"
#include "BeatClassification.h"
#include "SpeedFunction.h"
#include "StartTempoEstimator.h"
#include "BeatClassifier.h"

#include <boost/utility.hpp>

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
	void RegisterBeat(real_time_t const & beatTime, double prob);

	real_time_t StartTimeEstimate() { return startTempoEstimator_.StartTimeEstimate(); }
	speed_t SpeedEstimateAt(real_time_t const & time);

private:
	class ScoreEventBuilder;
	typedef EventBuffer<BeatClassification, real_time_t> BeatHistoryBuffer;
	typedef EventBuffer<Data::TempoSensitivityChange, score_time_t> TempoSensitivityBuffer;
	typedef EventBuffer<Data::Fermata, score_time_t> FermataBuffer;

private:
	BeatClassification ClassifyBeatAt(real_time_t const & time, double clarity);

	beat_pos_t BeatOffsetEstimate() const;
	score_time_t AccelerationTimeAt(score_time_t time);

private:
	TimeWarper const & timeWarper_;
	Follower & parent_;
	TempoMap tempoMap_;

	StartTempoEstimator startTempoEstimator_;
	BeatClassifier beatClassifier_;
	BeatHistoryBuffer beatHistory_;

	bool newBeats_;
	SpeedFunction acceleration_;

	TempoSensitivityBuffer tempoSensitivities_;
	FermataBuffer fermatas_;
};

} // namespace ScoreFollower
} // namespace cf
