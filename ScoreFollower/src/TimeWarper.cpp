#include "TimeWarper.h"

#define SPEED_RATIO 1000UL

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
	assert(time > realTime_);
	duration_t diff = time - realTime_;
	score_time_t progress = (diff.count() * speed_) / SPEED_RATIO;
	return scoreTime_ + progress;
}

TimeWarper::TimeWarper()
	: warpHistory_(100) // Arbitrary length, should be long enough...
{
}

void
TimeWarper::ReadTempoTrack(TrackReader<bool> & reader)
{
	// TODO
}

void
TimeWarper::FixTimeMapping(real_time_t const & realTime, score_time_t const & scoreTime)
{
	speed_t speed = CalculateSpeedAt(realTime);
	warpHistory_.RegisterEvent(realTime, WarpPoint(speed, realTime, scoreTime));
}

void
TimeWarper::RegisterBeat(real_time_t const & beatTime)
{
	// TODO
}

score_time_t
TimeWarper::WarpTimestamp(real_time_t const & time)
{
	 warpHistory_.DataLowerBoundInclusive(time);
}

TimeWarper::speed_t
TimeWarper::CalculateSpeedAt(real_time_t time)
{
	// TODO
	return SPEED_RATIO;
}


} // namespace ScoreFollower
} // namespace cf
