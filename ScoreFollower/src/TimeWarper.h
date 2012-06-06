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

	void ReadTempoTrack(TrackReader<tempo_t> & reader);
	void RegisterBeat(real_time_t const & beatTime);

	// Returns the score time that should "occur" at the real time
	score_time_t WarpTimestamp(real_time_t const & time);

	// Returns the real time at which the score time should "occur"
	// The reference parameter must be before or at the resulting real time,
	// but the closer it is, the more effective this is
	real_time_t InverseWarpTimestamp(score_time_t const & time, real_time_t const & reference);

	// Unoptimized version of the above, has to iterate through the whole warp history
	real_time_t InverseWarpTimestamp(score_time_t const & time);

private:
	typedef double speed_t;

	// class for storing warping history
	class WarpPoint
	{
	public:
		WarpPoint(speed_t speed, real_time_t realTime, score_time_t scoreTime);
		
		score_time_t Warp(real_time_t const & time) const;
		real_time_t InverseWarp(score_time_t const & time) const;

		real_time_t realTime() const { return realTime_; }
		score_time_t scoreTime() const { return scoreTime_; }

	private:
		// These are non-const so the class is assignable
		speed_t speed_;
		real_time_t realTime_;
		score_time_t scoreTime_;
	};

private:
	typedef EventBuffer<tempo_t, score_time_t, std::vector> TempoMap;
	typedef EventBuffer<WarpPoint, real_time_t> WarpHistoryBuffer;
	typedef EventBuffer<double, real_time_t> BeatHistoryBuffer;

private:
	// Actual implementation of inverse warping, used by the two public functions
	real_time_t InverseWarpTimestamp(score_time_t const & time, WarpHistoryBuffer::Range & searchRange);

	void FixTimeMapping(real_time_t const & realTime, score_time_t const & scoreTime);
	speed_t CalculateSpeedAt(real_time_t time);

private:
	TempoMap tempoMap_;
	WarpHistoryBuffer warpHistory_;
	BeatHistoryBuffer beatHistory_;
};

} // namespace ScoreFollower
} // namespace cf
