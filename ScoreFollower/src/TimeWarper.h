#pragma once

#include <boost/utility.hpp>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class TimeWarper : public boost::noncopyable
{
public:
	TimeWarper();

	void FixTimeMapping(
		real_time_t const & realTime,
		score_time_t const & scoreTime,
		speed_t const & speed);

	// return true if no mappings have been fixed
	bool Empty() const { return warpHistory_.AllEvents().Empty(); }

	// Returns the score time that should "occur" at the real time
	score_time_t WarpTimestamp(real_time_t const & time) const;

	// Returns the real time at which the score time should "occur"
	// The hint parameter should be before or at the resulting real time,
	// but the closer it is, the more effective this is.
	// If the hint is invalid, a full search is made
	real_time_t InverseWarpTimestamp(score_time_t const & time, real_time_t const & hint) const;

	// Unoptimized version of the above, has to iterate through the whole warp history
	real_time_t InverseWarpTimestamp(score_time_t const & time) const;

	speed_t SpeedAt(real_time_t const & time) const;

	// Do a static time warp without registering any points to any warper.
	static score_time_t WarpTimestamp(
		real_time_t const & time,
		real_time_t const & referenceRealTime,
		score_time_t const & referenceScoreTime,
		speed_t const & referenceSpeed);

private:
	// class for storing warping history
	class WarpPoint
	{
	public:
		WarpPoint(speed_t speed, real_time_t realTime, score_time_t scoreTime);
		
		score_time_t Warp(real_time_t const & time, bool allowBackwardsWarping = false) const;
		real_time_t InverseWarp(score_time_t const & time) const;

		real_time_t realTime() const { return realTime_; }
		score_time_t scoreTime() const { return scoreTime_; }
		speed_t speed() const { return speed_; }

	private:
		// These are non-const so the class is assignable
		speed_t speed_;
		real_time_t realTime_;
		score_time_t scoreTime_;
	};

private:
	typedef EventBuffer<WarpPoint, real_time_t> WarpHistoryBuffer;

	// Actual implementation of inverse warping, used by the two public functions
	real_time_t InverseWarpTimestamp(score_time_t const & time, WarpHistoryBuffer::Range & searchRange) const;

private:
	WarpHistoryBuffer warpHistory_;
};

} // namespace ScoreFollower
} // namespace cf
