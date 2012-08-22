#pragma once

#include <vector>

#include "cf/EventBuffer.h"
#include "cf/units_math.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/ScoreReader.h"

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

	void RegisterStartGestureLength(duration_t const & gestureDuration);
	void RegisterPreparatoryBeat(real_time_t const & time);
	void RegisterBeat(real_time_t const & beatTime, double prob);

	real_time_t StartTimeEstimate() { return startTempoEstimator_.StartTimeEstimate(); }
	speed_t SpeedEstimateAt(real_time_t const & time);

private:
	typedef EventBuffer<BeatClassification, real_time_t> BeatHistoryBuffer;

private:
	BeatClassification ClassifyBeatAt(real_time_t const & time, double clarity);

	beat_pos_t BeatOffsetEstimate() const;
	void UseStartTempoEstimateIfReady();

private:
	TimeWarper const & timeWarper_;
	Follower & parent_;
	TempoMap tempoMap_;

	StartTempoEstimator startTempoEstimator_;
	BeatClassifier beatClassifier_;
	BeatHistoryBuffer beatHistory_;

	bool newBeats_;
	SpeedFunction acceleration_;
};

} // namespace ScoreFollower
} // namespace cf
