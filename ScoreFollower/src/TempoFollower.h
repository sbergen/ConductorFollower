#pragma once

#include <vector>

#include <boost/utility.hpp>

#include "cf/EventBuffer.h"
#include "cf/units_math.h"
#include "cf/TimeFilters.h"

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

namespace cf {
namespace ScoreFollower {

class TimeWarper;
class FollowerImpl;

class TempoFollower : public boost::noncopyable
{
public:
	TempoFollower(TimeWarper const & timeWarper, FollowerImpl & parent);

	void ReadScore(ScoreReader & reader);
	void LearnPatterns(Data::PatternMap const & patterns) { beatClassifier_->LearnPatterns(patterns); }
	void LearnScoreEvents(Data::ScoreEventList const & events);

	void RegisterStartGesture(MotionTracker::StartGestureData const & data);
	BeatEvent RegisterBeat(real_time_t const & beatTime, double prob);

	real_time_t StartTimeEstimate() { return startTempoEstimator_.StartTimeEstimate(); }
	speed_t SpeedEstimateAt(real_time_t const & time);

	ScorePosition ScorePositionAt(real_time_t const & time) const;

public: // "Special" stuff
	void StartAtDefaultTempo();

private:
	class ScoreEventBuilder;
	typedef EventBuffer<Data::TempoSensitivityChange, score_time_t, std::vector> TempoSensitivityBuffer;

private:
	BeatClassification ClassifyBeatAt(real_time_t const & time, double clarity);
	score_time_t AccelerationTimeAt(score_time_t time, time_quantity beatInterval);

private: // Basic tempo following
	TimeWarper const & timeWarper_;
	FollowerImpl & parent_;
	TempoMap tempoMap_;

	StartTempoEstimator startTempoEstimator_;
	boost::shared_ptr<BeatClassifier> beatClassifier_;
	TempoFunction tempoFunction_;

	BeatClassification previousClassification_;
	real_time_t previousBeatTime_;
	tempo_t previousScoreTempo_;

	LinearDecayTimeFilter<tempo_t> tempoFilter_;

private: // Score events
	TempoSensitivityBuffer tempoSensitivities_;
};

} // namespace ScoreFollower
} // namespace cf
