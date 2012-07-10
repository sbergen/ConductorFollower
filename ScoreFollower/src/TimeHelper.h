#pragma once

#include <utility>
#include <boost/utility.hpp>

#include "ScoreFollower/types.h"

#include "AudioBlockTimeManager.h"
#include "TimeWarper.h"
#include "TempoFollower.h"

namespace cf {
namespace ScoreFollower {

class Follower;

class TimeHelper : public boost::noncopyable
{
public:
	typedef std::pair<score_time_t, score_time_t> ScoreTimeBlock;
	typedef std::pair<real_time_t, real_time_t> RealTimeBlock;

public:
	TimeHelper(Follower & parent);
	
	void SetBlockParameters(unsigned samplerate, unsigned blockSize);
	void ReadTempoTrack(TempoReaderPtr reader) { tempoFollower_.ReadTempoTrack(reader); }

	// These need to be clled in this order!
	void StartNewBlock();
	void FixScoreRange();

	void RegisterBeat(real_time_t const & time);

	unsigned ScoreTimeToFrameOffset(score_time_t const & time) const;

	RealTimeBlock const & CurrentRealTimeBlock() { return rtRange_; }
	ScoreTimeBlock const & CurrentScoreTimeBlock() { return scoreRange_; }

private:
	Follower & parent_;

	boost::shared_ptr<AudioBlockTimeManager> timeManager_;
	TimeWarper timeWarper_;
	TempoFollower tempoFollower_;

	RealTimeBlock rtRange_;
	ScoreTimeBlock scoreRange_;
	speed_t previousSpeed_;
};

} // namespace ScoreFollower
} // namespace cf