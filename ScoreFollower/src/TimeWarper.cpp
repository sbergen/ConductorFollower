#include "TimeWarper.h"

#include "cf/globals.h"

namespace cf {
namespace ScoreFollower {

TimeWarper::WarpPoint::WarpPoint(speed_t speed, real_time_t realTime, score_time_t scoreTime)
	: speed_(speed)
	, realTime_(realTime)
	, scoreTime_(scoreTime)
{
}

score_time_t
TimeWarper::WarpPoint::Warp(real_time_t const & time, bool allowBackwardsWarping) const
{
	if (!allowBackwardsWarping && (time < realTime_)) {
		throw std::runtime_error("Backwards warp not allowed!");
	}

	duration_t diff = time - realTime_;
	score_time_t progress = time::quantity_cast<score_time_t>(diff) * speed_;
	return scoreTime_ + progress;
}


real_time_t
TimeWarper::WarpPoint::InverseWarp(score_time_t const & time) const
{
	// Only allow warping after the fixed point
	assert(time >= scoreTime_);

	score_time_t diff = time - scoreTime_;
	score_time_t progress = diff / speed_;

	real_time_t result = realTime_ + time::duration_cast<duration_t>(progress);

	return result;
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
	// No warping done, return 0.0 for the first timestamps
	if (warpHistory_.AllEvents().Empty()) {
		return 0.0 * score::seconds;
	}

	// Regular warping
	auto history = warpHistory_.EventsSinceInclusive(time);
	if (!history.Empty()) {
		return history[0].data.Warp(time);
	}

	// Warp backwards for before-the-start events
	return warpHistory_.AllEvents().Front().data.Warp(time, true);
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

score_time_t
TimeWarper::WarpTimestamp(
	real_time_t const & time,
	real_time_t const & referenceRealTime,
	score_time_t const & referenceScoreTime,
	speed_t const & referenceSpeed)
{
	WarpPoint point(referenceSpeed, referenceRealTime, referenceScoreTime);
	return point.Warp(time, true);
}

real_time_t
TimeWarper::InverseWarpTimestamp(score_time_t const & time, WarpHistoryBuffer::Range & searchRange) const
{
	assert(!searchRange.Empty());
	assert(time >= searchRange[0].data.scoreTime());

	WarpPoint const * point = &searchRange[0].data;
	searchRange.ForEachWhile([&time, &point](real_time_t const &, WarpPoint const & p) -> bool
	{
		if (p.scoreTime() > time) { return false; }
		point = &p;
		return true;
	});

	return point->InverseWarp(time);
}

} // namespace ScoreFollower
} // namespace cf
