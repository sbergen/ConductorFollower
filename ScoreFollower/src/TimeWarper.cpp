#include "TimeWarper.h"

namespace cf {
namespace ScoreFollower {

TimeWarper::WarpPoint::WarpPoint(speed_t speed, real_time_t realTime, score_time_t scoreTime)
	: speed_(speed)
	, realTime_(realTime)
	, scoreTime_(scoreTime)
{
}

score_time_t
TimeWarper::WarpPoint::Warp(real_time_t const & time) const
{
	// Only allow warping after the fixed point
	assert(time >= realTime_);

	duration_t diff = time - realTime_;
	duration_t warpedDuration(static_cast<duration_t::rep>(diff.count() * speed_));

	score_time_t progress = boost::chrono::duration_cast<score_time_t>(warpedDuration);
	return scoreTime_ + progress;
}


real_time_t
TimeWarper::WarpPoint::InverseWarp(score_time_t const & time) const
{
	// Only allow warping after the fixed point
	assert(time >= scoreTime_);

	duration_t diff = time - scoreTime_;
	duration_t warpedDuration(static_cast<duration_t::rep>(diff.count() * (1.0 / speed_)));

	score_time_t progress = boost::chrono::duration_cast<score_time_t>(warpedDuration);
	return realTime_ + progress;
}

TimeWarper::TimeWarper()
	: warpHistory_(100) // Arbitrary length, should be long enough...
{
}

void
TimeWarper::FixTimeMapping(real_time_t const & realTime, score_time_t const & scoreTime, speed_t const & speed)
{
	assert(realTime > warpHistory_.AllEvents().LastTimestamp());
	warpHistory_.RegisterEvent(realTime, WarpPoint(speed, realTime, scoreTime));
}

score_time_t
TimeWarper::WarpTimestamp(real_time_t const & time)
{
	auto history = warpHistory_.EventsSinceInclusive(time);
	if (history.Empty()) {
		return score_time_t::zero();
	} else {
		return history.data().Warp(time);
	}
}

real_time_t
TimeWarper::InverseWarpTimestamp(score_time_t const & time, real_time_t const & reference)
{
	auto history = warpHistory_.EventsSinceInclusive(reference);
	return InverseWarpTimestamp(time, history);
}

real_time_t
TimeWarper::InverseWarpTimestamp(score_time_t const & time)
{
	auto history = warpHistory_.AllEvents();
	return InverseWarpTimestamp(time, history);
}

real_time_t
TimeWarper::InverseWarpTimestamp(score_time_t const & time, WarpHistoryBuffer::Range & searchRange)
{
	assert(!searchRange.Empty());
	assert(time >= searchRange.data().scoreTime());

	while (!searchRange.AtEnd()) {
		WarpPoint const & p = searchRange.data();

		// Check the next one (if it exists)
		if (searchRange.Next() && (time >= searchRange.data().scoreTime())) { continue; }
		return p.InverseWarp(time);
	}

	throw std::logic_error("Should never be reached...");
}

} // namespace ScoreFollower
} // namespace cf
