#pragma once

#include <vector>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"

namespace cf {
namespace ScoreFollower {

class TimeWarper
{
public:
	TimeWarper();

	void ReadTempoTrack(TrackReader<bool> & reader);

	void FixTimeMapping(real_time_t const & realTime, score_time_t const & scoreTime);
	void RegisterBeat(real_time_t const & beatTime);
	score_time_t WarpTimestamp(real_time_t const & time);

private:
	typedef EventBuffer<bool, score_time_t, std::vector> TempoBuffer;
	// The unit is in the cpp file...
	typedef unsigned long speed_t;

	TempoBuffer tempoBuffer_;

	speed_t currentSpeed_;

	score_time_t fixedScoreTime_;
	real_time_t  fixedRealTime_;

};

} // namespace ScoreFollower
} // namespace cf
