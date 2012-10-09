#pragma once

#include <utility>
#include <boost/utility.hpp>

#include "PatchMapper/ConductorContext.h"
#include "Data/BeatPatternParser.h"
#include "MotionTracker/StartGestureData.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/StatusEvents.h"
#include "ScoreFollower/FollowerStatus.h"

#include "AudioBlockTimeManager.h"
#include "TimeWarper.h"
#include "TempoFollower.h"

namespace cf {
namespace ScoreFollower {

class FollowerImpl;

class TimeHelper : public boost::noncopyable
{
public:
	typedef std::pair<score_time_t, score_time_t> ScoreTimeBlock;
	typedef std::pair<real_time_t, real_time_t> RealTimeBlock;

	struct TempoInfo
	{
		tempo_t score;
		tempo_t current;
	};

public:
	TimeHelper(FollowerImpl & parent, PatchMapper::ConductorContext & conductorContext);
	boost::shared_ptr<TimeHelper> FreshClone();
	
	void SetBlockParameters(unsigned samplerate, unsigned blockSize);
	void ReadScore(ScoreReader & reader) { tempoFollower_.ReadScore(reader); }
	void LearnPatterns(Data::PatternMap const & patterns) { tempoFollower_.LearnPatterns(patterns); }
	void LearnScoreEvents(Data::ScoreEventList const & events) { tempoFollower_.LearnScoreEvents(events); }

	// These need to be clled in this order!
	void StartNewBlock();
	void FixScoreRange(Status::FollowerStatus & status);

	void RegisterStartGesture(MotionTracker::StartGestureData const & data);
	BeatEvent RegisterBeat(real_time_t const & time, double prob);

	real_time_t StartTimeEstimate() { return tempoFollower_.StartTimeEstimate(); }

	// frame offset in current audio block
	samples_t ScoreTimeToFrameOffset(score_time_t const & time) const;

	speed_t CurrentSpeed() const { return previousSpeed_; }
	RealTimeBlock const & CurrentRealTimeBlock() { return rtRange_; }
	ScoreTimeBlock const & CurrentScoreTimeBlock() { return scoreRange_; }

	ScorePosition ScorePositionAt(real_time_t const & time) const
	{
		return tempoFollower_.ScorePositionAt(time);
	}

	TempoInfo CurrentTempoInfo();

private:
	FollowerImpl & parent_;
	PatchMapper::ConductorContext & conductorContext_;

	boost::shared_ptr<AudioBlockTimeManager> timeManager_;
	TimeWarper timeWarper_;
	TempoFollower tempoFollower_;

	RealTimeBlock rtRange_;
	ScoreTimeBlock scoreRange_;
	speed_t previousSpeed_;
};

} // namespace ScoreFollower
} // namespace cf