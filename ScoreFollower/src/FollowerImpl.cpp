#include "FollowerImpl.h"

#include "MotionTracker/EventProvider.h"
#include "MotionTracker/EventThrottler.h"

#include "FeatureExtractor/Extractor.h"

#include "ScoreFollower/ScoreEventHandle.h"
#include "ScoreFollower/ScoreEventManipulator.h"

#include "TimeWarper.h"
#include "TempoFollower.h"
#include "AudioBlockTimeManager.h"
#include "globals.h"

using namespace cf::FeatureExtractor;
using namespace cf::MotionTracker;

namespace cf {
namespace ScoreFollower {

boost::shared_ptr<Follower>
Follower::Create(unsigned samplerate, unsigned blockSize)
{
	return boost::shared_ptr<Follower>(new FollowerImpl(samplerate, blockSize));
}

FollowerImpl::FollowerImpl(unsigned samplerate, unsigned blockSize)
	: timeManager_(samplerate, blockSize)
	, tempoFollower_(timeWarper_, *this) 
	, startRollingTime_(real_time_t::max())
	, velocity_(0.5)
{
	eventProvider_.reset(EventProvider::Create());
	eventThrottler_.reset(new EventThrottler(*eventProvider_));
	featureExtractor_.reset(Extractor::Create());
}

FollowerImpl::~FollowerImpl()
{
}

void
FollowerImpl::CollectData(boost::shared_ptr<ScoreReader> scoreReader)
{
	tempoFollower_.ReadTempoTrack(scoreReader->TempoTrack());
	scoreHelper_.CollectData(scoreReader);

	{ // TODO handle state better
		state_ = WaitingForStartGesture;
		startGestureConnection_ = featureExtractor_->StartGestureDetected.connect(
			boost::bind(&FollowerImpl::GotStartGesture, this, _1, _2));
		assert(eventProvider_->StartProduction());
	}
}

void
FollowerImpl::StartNewBlock()
{
	std::pair<score_time_t, score_time_t> scoreRange;
	auto currentBlock = timeManager_.GetRangeForNow();
	
	eventThrottler_->ConsumeEventsUntil(
		boost::bind(&FollowerImpl::ConsumeEvent, this, _1),
		currentBlock.first);

	if (state_ == WaitingForStartGesture && currentBlock.first >= startRollingTime_) {
		state_ = Rolling;
		beatConnection_ = featureExtractor_->BeatDetected.connect(
			boost::bind(&FollowerImpl::GotBeat, this, _1));
	}

	if (state_ != Rolling) { return; }

	// Get start estimate based on old data
	scoreRange.first = timeWarper_.WarpTimestamp(currentBlock.first);

	// Fix the starting point, ensures the next warp is "accurate"
	speed_t speed = tempoFollower_.SpeedEstimateAt(currentBlock.first);
	if (speed != previousSpeed_) {
		status_.SetValue<Status::Speed>(speed);
		previousSpeed_ = speed;
		timeWarper_.FixTimeMapping(currentBlock.first, scoreRange.first, speed);
	}

	// Now use the new estimate for this block
	scoreRange.second = timeWarper_.WarpTimestamp(currentBlock.second);

	if (scoreRange.first != score_time_t::zero()) {
		assert(scoreRange_.second == scoreRange.first);
	}
	scoreRange_ = scoreRange;
}

void
FollowerImpl::GetTrackEventsForBlock(unsigned track, ScoreEventManipulator & manipulator, BlockBuffer & events)
{
	auto ev = scoreHelper_[track].EventsBetween(scoreRange_.first, scoreRange_.second);

	events.Clear();
	if (state_ != Rolling) { return; }

	ev.ForEach(boost::bind(&FollowerImpl::CopyEventToBuffer, this, _1, _2, boost::ref(manipulator), boost::ref(events)));
}

void
FollowerImpl::CopyEventToBuffer(score_time_t const & time, ScoreEventHandle const & data, ScoreEventManipulator & manipulator, BlockBuffer & events)  const
{
	unsigned frameOffset = ScoreTimeToFrameOffset(time);
	double velocity = NewVelocityAt(manipulator.GetVelocity(data), time);
	
	// TODO fugly const modification, think about this...
	ScoreEventHandle ev(data);
	manipulator.ApplyVelocity(ev, velocity);
	events.RegisterEvent(frameOffset, ev);
}

unsigned
FollowerImpl::ScoreTimeToFrameOffset(score_time_t const & time) const
{
	real_time_t const & ref = timeManager_.CurrentBlockStart();
	real_time_t realTime = timeWarper_.InverseWarpTimestamp(time, ref);

	// There are rounding errors sometimes, so allow 10us of "jitter" here
	// This is perfectly acceptable :)
	time::limitRange(realTime,
		timeManager_.CurrentBlockStart(), timeManager_.CurrentBlockEnd(),
		boost::chrono::microseconds(10));

	return timeManager_.ToSampleOffset(realTime);
}

double
FollowerImpl::NewVelocityAt(double oldVelocity, score_time_t const & time) const
{
	// TODO use time and something fancier :)
	return (oldVelocity + (velocity_ - 0.5));
}

void
FollowerImpl::GotStartGesture(real_time_t const & beatTime, real_time_t const & startTime)
{
	startGestureConnection_.disconnect();

	tempoFollower_.RegisterBeat(beatTime);
	startRollingTime_ = startTime;
}

void
FollowerImpl::GotBeat(real_time_t const & time)
{
	assert(time < timeManager_.CurrentBlockStart());
	tempoFollower_.RegisterBeat(time);
}

void
FollowerImpl::ConsumeEvent(Event const & e)
{
	// TODO clean up!

	switch(e.type())
	{
	case Event::TrackingStarted:
		status_.SetValue<Status::Running>(true);
		break;
	case Event::TrackingEnded:
		status_.SetValue<Status::Running>(false);
		break;
	case Event::Position:
		featureExtractor_->RegisterPosition(e.timestamp(), e.data<Point3D>());
		HandleNewPosition(e.timestamp());
		break;
	}
}

void
FollowerImpl::HandleNewPosition(real_time_t const & timestamp)
{
	UpdateMagnitude(timestamp);
}

void
FollowerImpl::UpdateMagnitude(real_time_t const & timestamp)
{
	// Make better
	Point3D distance = featureExtractor_->MagnitudeOfMovementSince(timestamp - milliseconds_t(1500));
	coord_t magnitude = geometry::abs(distance);
	status_.SetValue<Status::MagnitudeOfMovement>(magnitude);
	velocity_ = magnitude / 600;
}

} // namespace ScoreFollower
} // namespace cf
