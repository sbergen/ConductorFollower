#include "ScoreFollower/Follower-private.h"

#include "MotionTracker/EventProvider.h"
#include "FeatureExtractor/Extractor.h"

#include "TimeWarper.h"
#include "TempoFollower.h"
#include "AudioBlockTimeManager.h"
#include "globals.h"

using namespace cf::FeatureExtractor;
using namespace cf::MotionTracker;

namespace cf {
namespace ScoreFollower {

FollowerPrivate::FollowerPrivate(unsigned samplerate, unsigned blockSize)
	: started_(false)
	, rolling_(false)
	, previousBeat_(real_time_t::min())
	, velocity_(0.5)
	, gestureBuffer_(128)
{
	globalsInit_.reset(new GlobalsInitializer());

	eventProvider_.reset(EventProvider::Create());
	featureExtractor_.reset(Extractor::Create());
	timeManager_.reset(new AudioBlockTimeManager(samplerate, blockSize));
	timeWarper_.reset(new TimeWarper());
	tempoFollower_.reset(new TempoFollower(*timeWarper_, *this));
}

FollowerPrivate::~FollowerPrivate()
{
}

void
FollowerPrivate::ReadTempoTrack(TrackReader<tempo_t> & reader)
{
	tempoFollower_->ReadTempoTrack(reader);
}

void
FollowerPrivate::StartNewBlock(std::pair<score_time_t, score_time_t> & scoreRange)
{
	auto currentBlock = timeManager_->GetRangeForNow();
	EnsureProperStart();
	ConsumeEvents();

	if (!rolling_) { return; }

	// Get start estimate based on old data
	scoreRange.first = timeWarper_->WarpTimestamp(currentBlock.first);

	// Fix the starting point, ensures the next warp is "accurate"
	speed_t speed = tempoFollower_->SpeedEstimateAt(currentBlock.first);
	if (speed != previousSpeed_) {
		status_.SetValue<Status::Speed>(speed);
		previousSpeed_ = speed;
		timeWarper_->FixTimeMapping(currentBlock.first, scoreRange.first, speed);
	}

	// Now use the new estimate for this block
	scoreRange.second = timeWarper_->WarpTimestamp(currentBlock.second);

	if (scoreRange.first != score_time_t::zero()) {
		assert(prevScoreRange_.second == scoreRange.first);
	}
	prevScoreRange_ = scoreRange;
	
}

unsigned
FollowerPrivate::ScoreTimeToFrameOffset(score_time_t const & time)
{
	real_time_t const & ref = timeManager_->CurrentBlockStart();
	real_time_t realTime = timeWarper_->InverseWarpTimestamp(time, ref);

	// There are rounding errors sometimes, so allow 10us of "jitter" here
	// This is perfectly acceptable :)
	time::limitRange(realTime,
		timeManager_->CurrentBlockStart(), timeManager_->CurrentBlockEnd(),
		boost::chrono::microseconds(10));

	return timeManager_->ToSampleOffset(realTime);
}

double
FollowerPrivate::NewVelocityAt(double oldVelocity, score_time_t const & time)
{
	// TODO use time and something fancier :)
	return (oldVelocity + (velocity_ - 0.5));
}

void
FollowerPrivate::EnsureProperStart()
{
	if (started_) { return; }
	started_ = true;

	assert(eventProvider_->StartProduction());
}

void
FollowerPrivate::ConsumeEvents()
{
	if (queuedEvent_.isQueued) {
		ConsumeEvent(queuedEvent_.e);
		queuedEvent_.isQueued = false;
	}

	while (eventProvider_->DequeueEvent(queuedEvent_.e)) {
		if (queuedEvent_.e.timestamp() >= timeManager_->CurrentBlockStart()) {
			queuedEvent_.isQueued = true;
			break;
		}
		ConsumeEvent(queuedEvent_.e);
	}
}

void
FollowerPrivate::ConsumeEvent(Event const & e)
{
	// TODO clean up!

	switch(e.type())
	{
	case Event::TrackingStarted:
		rolling_ = true;
		status_.SetValue<Status::Running>(true);
		break;
	case Event::TrackingEnded:
		status_.SetValue<Status::Running>(false);
		break;
	case Event::Position:
		featureExtractor_->RegisterPosition(e.timestamp(), e.data<Point3D>());
		HandlePossibleNewBeats();

		// TODO move elsewhere, use some beat related time
		Point3D distance = featureExtractor_->MagnitudeOfMovementSince(e.timestamp() - milliseconds_t(1000));
		coord_t magnitude = geometry::abs(distance);
		status_.SetValue<Status::MagnitudeOfMovement>(magnitude);
		velocity_ = magnitude / 600;

		break;
		/*
	case Event::Apex:
		//tempoFollower_->RegisterApex(e.timestamp());
		break;
	case Event::Magnitude:
		// TODO store timestamps...
		Point3D distance = e.data<Point3D>();
		coord_t magnitude = geometry::abs(distance);
		status_.SetValue<Status::MagnitudeOfMovement>(magnitude);
		velocity_ = magnitude / 600;
		break;
		*/
	}
}

void
FollowerPrivate::HandlePossibleNewBeats()
{
	real_time_t since = previousBeat_ + milliseconds_t(1);
	featureExtractor_->GetBeatsSince(since, gestureBuffer_);

	auto beats = gestureBuffer_.AllEvents();
	while (!beats.AtEnd()) {
		assert(beats.timestamp() < timeManager_->CurrentBlockStart());
		tempoFollower_->RegisterBeat(beats.timestamp());
		previousBeat_ = beats.timestamp();
		beats.Next();
	}
}

} // namespace ScoreFollower
} // namespace cf
