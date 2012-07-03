#include "FollowerImpl.h"

#include "MotionTracker/EventProvider.h"
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
	: started_(false)
	, rolling_(false)
	, gotStartGesture_(false)
	, previousBeat_(real_time_t::min())
	, startRollingTime_(real_time_t::max())
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

FollowerImpl::~FollowerImpl()
{
}

void
FollowerImpl::CollectData(boost::shared_ptr<ScoreReader> scoreReader)
{
	scoreReader_ = scoreReader;
	
	// Tempo
	tempoFollower_->ReadTempoTrack(scoreReader->TempoTrack());

	// Tracks
	score_time_t timestamp;
	ScoreEventHandle data;
	for (int i = 0; i < scoreReader->TrackCount(); ++i) {
		trackBuffers_.push_back(TrackBuffer(0));
		auto reader = scoreReader->Track(i);
		while (reader->NextEvent(timestamp, data)) {
			trackBuffers_[i].RegisterEvent(timestamp, data);
		}
	}
}

void
FollowerImpl::StartNewBlock()
{
	std::pair<score_time_t, score_time_t> scoreRange;
	auto currentBlock = timeManager_->GetRangeForNow();
	EnsureProperStart();
	ConsumeEvents();

	// TODO: Does not do intra-buffer estimation!
	rolling_ = (currentBlock.first >= startRollingTime_);

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
		assert(scoreRange_.second == scoreRange.first);
	}
	scoreRange_ = scoreRange;
}

void
FollowerImpl::GetTrackEventsForBlock(unsigned track, ScoreEventManipulator & manipulator, BlockBuffer & events)
{
	assert(track < trackBuffers_.size());
	auto ev = trackBuffers_[track].EventsBetween(scoreRange_.first, scoreRange_.second);

	events.Clear();
	if (!rolling_) { return; }

	ev.ForEach(boost::bind(&FollowerImpl::CopyEventToBuffer, this, _1, _2, boost::ref(manipulator), boost::ref(events)));
}

void
FollowerImpl::CopyEventToBuffer(score_time_t const & time, ScoreEventHandle const & data, ScoreEventManipulator & manipulator, BlockBuffer & events)  const
{
	unsigned frameOffset = ScoreTimeToFrameOffset(time);
	double velocity = NewVelocityAt(manipulator.GetVelocity(data), time);
	
	// TODO fugly const modification, think about this...
	ScoreEventHandle ev = ScoreEventHandle(data);
	manipulator.ApplyVelocity(ev, velocity);
	events.RegisterEvent(frameOffset, ev);
}

unsigned
FollowerImpl::ScoreTimeToFrameOffset(score_time_t const & time) const
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
FollowerImpl::NewVelocityAt(double oldVelocity, score_time_t const & time) const
{
	// TODO use time and something fancier :)
	return (oldVelocity + (velocity_ - 0.5));
}

void
FollowerImpl::EnsureProperStart()
{
	if (started_) { return; }
	started_ = true;

	assert(eventProvider_->StartProduction());
}

void
FollowerImpl::ConsumeEvents()
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
	HandlePossibleNewBeats();
	UpdateMagnitude(timestamp);
	HandleStartGesture();
}

void
FollowerImpl::HandleStartGesture()
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
	duration_t gestureLength = apexes[0].timestamp - previousBeat_;
	seconds_t minTempo(60.0 / 40 / 2);
	seconds_t maxTempo(60.0 / 200 / 2);
	if (gestureLength > minTempo || gestureLength < maxTempo) { return; }

	// Done!
	startRollingTime_ = apexes[0].timestamp + gestureLength;
	tempoFollower_->RegisterBeat(previousBeat_);
	gotStartGesture_ = true;
}

void
FollowerImpl::HandlePossibleNewBeats()
{
	real_time_t since = previousBeat_ + milliseconds_t(1);
	featureExtractor_->GetBeatsSince(since, gestureBuffer_);

	gestureBuffer_.AllEvents().ForEach(
		[this](timestamp_t const & timestamp, double data)
	{
		assert(timestamp < timeManager_->CurrentBlockStart());
		if (gotStartGesture_) {
			tempoFollower_->RegisterBeat(timestamp);
		}
		previousBeat_ = timestamp;
	});
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
