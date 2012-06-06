#pragma once

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class TimeWarper
{
public:
	TimeWarper();

	void FixTimeMapping(
		real_time_t const & realTime,
		score_time_t const & scoreTime,
		speed_t const & speed);

	// Returns the score time that should "occur" at the real time
	score_time_t WarpTimestamp(real_time_t const & time);

	// Returns the real time at which the score time should "occur"
	// The reference parameter must be before or at the resulting real time,
	// but the closer it is, the more effective this is
	real_time_t InverseWarpTimestamp(score_time_t const & time, real_time_t const & reference);

	// Unoptimized version of the above, has to iterate through the whole warp history
	real_time_t InverseWarpTimestamp(score_time_t const & time);

private:
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
	typedef EventBuffer<WarpPoint, real_time_t> WarpHistoryBuffer;

	// Actual implementation of inverse warping, used by the two public functions
	real_time_t InverseWarpTimestamp(score_time_t const & time, WarpHistoryBuffer::Range & searchRange);

private:
	WarpHistoryBuffer warpHistory_;
};

} // namespace ScoreFollower
} // namespace cf
