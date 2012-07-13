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
	: warpHistory_(128) // Arbitrary length, should be long enough...
{
}

void
TimeWarper::FixTimeMapping(real_time_t const & realTime, score_time_t const & scoreTime, speed_t const & speed)
{
	assert(realTime > warpHistory_.AllEvents().LastTimestamp());
	warpHistory_.RegisterEvent(realTime, WarpPoint(speed, realTime, scoreTime));
}

score_time_t
TimeWarper::WarpTimestamp(real_time_t const & time) const
{
	auto history = warpHistory_.EventsSinceInclusive(time);
	if (history.Empty()) {
		return score_time_t::zero();
	} else {
		return history[0].data.Warp(time);
	}
}

real_time_t
TimeWarper::InverseWarpTimestamp(score_time_t const & time, real_time_t const & hint) const
{
	auto history = warpHistory_.EventsSinceInclusive(hint);
	if(time >= history[0].data.scoreTime()) {
		return InverseWarpTimestamp(time);
	} else {
		return InverseWarpTimestamp(time, history);
	}
}

real_time_t
TimeWarper::InverseWarpTimestamp(score_time_t const & time) const
{
	auto history = warpHistory_.AllEvents();
	return InverseWarpTimestamp(time, history);
}

speed_t
TimeWarper::SpeedAt(real_time_t const & time) const
{
	auto events = warpHistory_.EventsSinceInclusive(time);
	if (events.Empty()) { return speed_t(1.0); }
	return events[0].data.speed();
}

real_time_t
TimeWarper::InverseWarpTimestamp(score_time_t const & time, WarpHistoryBuffer::Range & searchRange) const
{
	assert(!searchRange.Empty());
	assert(time >= searchRange[0].data.scoreTime());

	WarpPoint const * point = &searchRange[0].data;
	searchRange.ForEachWhile([&time, &point](real_time_t const &, WarpPoint const & p) -> bool
	{
		if (time >= p.scoreTime()) { return false; }
		point = &p;
		return true;
	});

	return point->InverseWarp(time);
}

} // namespace ScoreFollower
} // namespace cf
