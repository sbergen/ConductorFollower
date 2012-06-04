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
TimeWarper::ReadTempoTrack(TrackReader<tempo_t> & reader)
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
	auto history = warpHistory_.EventsSinceInclusive(time);
	assert(!history.Empty());
	return history.data().Warp(time);
}

real_time_t
TimeWarper::InverseWarpTimestamp(real_time_t const & reference, score_time_t const & time)
{
	auto history = warpHistory_.EventsSinceInclusive(reference);
	assert(!history.Empty());
	return history.data().InverseWarp(time);
}

TimeWarper::speed_t
TimeWarper::CalculateSpeedAt(real_time_t time)
{
	// TODO
	return 1.0;
}


} // namespace ScoreFollower
} // namespace cf
