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
	: startRollingTime_(real_time_t::max())
	, velocity_(0.5)
{
	timeHelper_.reset(new TimeHelper(*this, samplerate, blockSize));
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
	timeHelper_->ReadTempoTrack(scoreReader->TempoTrack());
	scoreHelper_.CollectData(scoreReader);

	{ // TODO handle state better
		SetState(FollowerState::WaitingForCalibration);
		//state_ = WaitingForStartGesture;
		startGestureConnection_ = featureExtractor_->StartGestureDetected.connect(
			boost::bind(&FollowerImpl::GotStartGesture, this, _1, _2));
		assert(eventProvider_->StartProduction());
	}
}

void
FollowerImpl::StartNewBlock()
{
	// Start new RT block
	timeHelper_->StartNewBlock();
	auto const & currentBlock = timeHelper_->CurrentRealTimeBlock();

	// Consume events until the start of this block
	eventThrottler_->ConsumeEventsUntil(
		boost::bind(&FollowerImpl::ConsumeEvent, this, _1),
		currentBlock.first);

	// Check state
	if (State() == FollowerState::WaitingForStart && currentBlock.first >= startRollingTime_) {
		SetState(FollowerState::Rolling);
		beatConnection_ = featureExtractor_->BeatDetected.connect(
			boost::bind(&TimeHelper::RegisterBeat, timeHelper_, _1));
	}

	if (State() != FollowerState::Rolling) { return; }

	// If rolling, fix score range
	timeHelper_->FixScoreRange();
}

void
FollowerImpl::GetTrackEventsForBlock(unsigned track, ScoreEventManipulator & manipulator, BlockBuffer & events)
{
	auto scoreRange = timeHelper_->CurrentScoreTimeBlock();
	auto ev = scoreHelper_[track].EventsBetween(scoreRange.first, scoreRange.second);

	events.Clear();
	if (State() != FollowerState::Rolling) { return; }

	ev.ForEach(boost::bind(&FollowerImpl::CopyEventToBuffer, this, _1, _2, boost::ref(manipulator), boost::ref(events)));
}

FollowerState
FollowerImpl::State()
{
	FollowerState ret;
	status_.GetValue<Status::State>(ret);
	return ret;
}

void
FollowerImpl::SetState(FollowerState::Value state)
{
	status_.SetValue<Status::State>(state);
}

void
FollowerImpl::CopyEventToBuffer(score_time_t const & time, ScoreEventHandle const & data, ScoreEventManipulator & manipulator, BlockBuffer & events)  const
{
	unsigned frameOffset = timeHelper_->ScoreTimeToFrameOffset(time);
	double velocity = NewVelocityAt(manipulator.GetVelocity(data), time);
	
	// TODO fugly const modification, think about this...
	ScoreEventHandle ev(data);
	manipulator.ApplyVelocity(ev, velocity);
	events.RegisterEvent(frameOffset, ev);
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

	timeHelper_->RegisterBeat(beatTime);
	startRollingTime_ = startTime;
}

void
FollowerImpl::ConsumeEvent(Event const & e)
{
	switch(e.type())
	{
	case Event::TrackingStarted:
		SetState(FollowerState::WaitingForStart);
		break;
	case Event::TrackingEnded:
		SetState(FollowerState::Stopped);
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
