#include "TimeHelper.h"

#include "cf/globals.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerStatus.h"

namespace cf {
namespace ScoreFollower {

TimeHelper::TimeHelper(Follower & parent, PatchMapper::ConductorContext & conductorContext)
	: parent_(parent)
	, conductorContext_(conductorContext)
	, tempoFollower_(timeWarper_, parent) 
{
}

boost::shared_ptr<TimeHelper>
TimeHelper::FreshClone()
{
	auto clone = boost::make_shared<TimeHelper>(parent_, conductorContext_);
	clone->timeManager_ = timeManager_;
	return clone;
}

void
TimeHelper::SetBlockParameters(unsigned samplerate, unsigned blockSize)
{
	timeManager_ = boost::make_shared<AudioBlockTimeManager>(
		samplerate * score::samples_per_second,
		blockSize * score::samples);
}

void
TimeHelper::StartNewBlock()
{
	rtRange_ = timeManager_->GetRangeForNow();
}

void
TimeHelper::FixScoreRange(Follower::StatusRCU::WriterHandle & statusWriter)
{
	// Get start estimate based on old data or start gesture
	if (timeWarper_.Empty()) {
		auto startTime = tempoFollower_.StartTimeEstimate();
		auto startSpeed = tempoFollower_.SpeedEstimateAt(startTime);
		scoreRange_.first = TimeWarper::WarpTimestamp(rtRange_.first,
			startTime, 0.0 * score::seconds, startSpeed);
	} else {
		scoreRange_.first = timeWarper_.WarpTimestamp(rtRange_.first);
	}

	// Fix the starting point, ensures the next warp is "accurate"
	speed_t speed = tempoFollower_.SpeedEstimateAt(rtRange_.first);
	if (speed != previousSpeed_) {
		previousSpeed_ = speed;
		statusWriter->SetValue<Status::Speed>(speed);
		conductorContext_.tempo = speed;
		timeWarper_.FixTimeMapping(rtRange_.first, scoreRange_.first, speed);
	}

	// Now use the new estimate for this block
	scoreRange_.second = timeWarper_.WarpTimestamp(rtRange_.second);
	//LOG("Score range for audio block: %1% to %2%", scoreRange_.first, scoreRange_.second);
}

void
TimeHelper::RegisterStartGesture(MotionTracker::StartGestureData const & data)
{
	tempoFollower_.RegisterStartGesture(data);
}

void
TimeHelper::RegisterBeat(real_time_t const & time, double prob)
{
	assert(time < timeManager_->CurrentBlockStart());
	tempoFollower_.RegisterBeat(time, prob);
}

samples_t
TimeHelper::ScoreTimeToFrameOffset(score_time_t const & time) const
{
	real_time_t const & ref = timeManager_->CurrentBlockStart();
	real_time_t realTime = timeWarper_.InverseWarpTimestamp(time, ref);

	// There are rounding errors sometimes, so allow 10us of "jitter" here
	// This is perfectly acceptable :)
	time::limitRange(realTime,
		timeManager_->CurrentBlockStart(), timeManager_->CurrentBlockEnd(),
		boost::chrono::microseconds(10));

	return timeManager_->ToSampleOffset(realTime);
}

} // namespace ScoreFollower
} // namespace cf