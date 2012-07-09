#include "FollowerImpl.h"

#include <boost/make_shared.hpp>

#include "MotionTracker/EventProvider.h"
#include "MotionTracker/EventThrottler.h"

#include "FeatureExtractor/Extractor.h"

#include "ScoreFollower/ScoreEvent.h"

#include "TimeWarper.h"
#include "TempoFollower.h"
#include "AudioBlockTimeManager.h"
#include "globals.h"

using namespace cf::FeatureExtractor;
using namespace cf::MotionTracker;

namespace cf {
namespace ScoreFollower {

boost::shared_ptr<Follower>
Follower::Create(unsigned samplerate, unsigned blockSize, boost::shared_ptr<ScoreReader> scoreReader)
{
	return boost::make_shared<FollowerImpl>(samplerate, blockSize, scoreReader);
}

FollowerImpl::FollowerImpl(unsigned samplerate, unsigned blockSize, boost::shared_ptr<ScoreReader> scoreReader)
	: status_(boost::make_shared<Status::FollowerStatus>())
	, options_(boost::make_shared<Options::FollowerOptions>())
	, scoreReader_(scoreReader)
	, startRollingTime_(real_time_t::max())
{
	timeHelper_ = boost::make_shared<TimeHelper>(*this, samplerate, blockSize);
	eventProvider_= EventProvider::Create();
	eventThrottler_ = boost::make_shared<EventThrottler>(*eventProvider_);
	featureExtractor_ = Extractor::Create();
	scoreHelper_ = boost::make_shared<ScoreHelper>(timeHelper_);
}

FollowerImpl::~FollowerImpl()
{
}

void
FollowerImpl::CollectData(boost::shared_ptr<ScoreReader> scoreReader)
{
	timeHelper_->ReadTempoTrack(scoreReader->TempoTrack());
	scoreHelper_->LearnScore(scoreReader);

	{ // TODO handle state better
		SetState(FollowerState::WaitingForCalibration);
		startGestureConnection_ = featureExtractor_->StartGestureDetected.connect(
			boost::bind(&FollowerImpl::GotStartGesture, this, _1, _2));
		assert(eventProvider_->StartProduction());
	}
}

unsigned
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

	if (State() != FollowerState::Rolling) { return 0; }

	// If rolling, fix score range
	timeHelper_->FixScoreRange();

	return scoreReader_->TrackCount();
}

void
FollowerImpl::GetTrackEventsForBlock(unsigned track, BlockBuffer & events)
{
	events.Clear();
	if (State() != FollowerState::Rolling) { return; }

	scoreHelper_->GetTrackEventsForBlock(track, events);
}

FollowerState
FollowerImpl::State()
{
	return state_;
}

void
FollowerImpl::SetState(FollowerState::Value state)
{
	state_ = state;
	status_.write()->SetValue<Status::State>(state);
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
	status_.write()->SetValue<Status::MagnitudeOfMovement>(magnitude);
	scoreHelper_->SetVelocityFromMotion(magnitude / 600);
}

} // namespace ScoreFollower
} // namespace cf
