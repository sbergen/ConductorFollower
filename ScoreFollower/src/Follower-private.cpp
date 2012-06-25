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
	, gotStartGesture_(false)
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
FollowerPrivate::HandleNewPosition(real_time_t const & timestamp)
{
	HandlePossibleNewBeats();
	UpdateMagnitude(timestamp);
	HandleStartGesture();
}

void
FollowerPrivate::HandleStartGesture()
{
	if (gotStartGesture_) { return; }

	// TODO move all constants elsewhere!

	if (previousBeat_ == real_time_t::min()) { return; }

	// Check apexes
	featureExtractor_->GetApexesSince(previousBeat_, gestureBuffer_);
	auto apexes = gestureBuffer_.AllEvents();
	if (apexes.Empty()) { return; }

	// Check y-movement magnitude
	auto magnitude = featureExtractor_->MagnitudeOfMovementSince(previousBeat_);
	if (magnitude.get<1>() < 200) { return; }

	// Check duration
	duration_t gestureLength = apexes.timestamp() - previousBeat_;
	seconds_t minTempo(60.0 / 40 / 2);
	seconds_t maxTempo(60.0 / 200 / 2);
	if (gestureLength > minTempo || gestureLength < maxTempo) { return; }

	// Done!
	tempoFollower_->RegisterBeat(previousBeat_);
	gotStartGesture_ = true;
}

void
FollowerPrivate::HandlePossibleNewBeats()
{
	real_time_t since = previousBeat_ + milliseconds_t(1);
	featureExtractor_->GetBeatsSince(since, gestureBuffer_);

	auto beats = gestureBuffer_.AllEvents();
	while (!beats.AtEnd()) {
		assert(beats.timestamp() < timeManager_->CurrentBlockStart());
		if (gotStartGesture_) {
			tempoFollower_->RegisterBeat(beats.timestamp());
			rolling_ = true;
		}
		previousBeat_ = beats.timestamp();
		beats.Next();
	}
}

void
FollowerPrivate::UpdateMagnitude(real_time_t const & timestamp)
{
	// Make better
	Point3D distance = featureExtractor_->MagnitudeOfMovementSince(timestamp - milliseconds_t(1500));
	coord_t magnitude = geometry::abs(distance);
	status_.SetValue<Status::MagnitudeOfMovement>(magnitude);
	velocity_ = magnitude / 600;
}

} // namespace ScoreFollower
} // namespace cf
