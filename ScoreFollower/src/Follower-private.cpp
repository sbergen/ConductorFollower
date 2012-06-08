#include "ScoreFollower/Follower-private.h"

#include "FeatureExtractor/EventProvider.h"

#include "TimeWarper.h"
#include "TempoFollower.h"
#include "AudioBlockTimeManager.h"
#include "globals.h"

using namespace cf::FeatureExtractor;

namespace cf {
namespace ScoreFollower {

FollowerTypeIndependentImpl::FollowerTypeIndependentImpl(unsigned samplerate, unsigned blockSize)
	: started_(false)
	, rolling_(false)
{
	globalsInit_.reset(new GlobalsInitializer());

	eventProvider_.reset(EventProvider::Create());
	timeManager_.reset(new AudioBlockTimeManager(samplerate, blockSize));
	timeWarper_.reset(new TimeWarper());
	tempoFollower_.reset(new TempoFollower());
}

FollowerTypeIndependentImpl::~FollowerTypeIndependentImpl()
{
}

void
FollowerTypeIndependentImpl::ReadTempoTrack(TrackReader<tempo_t> & reader)
{
	tempoFollower_->ReadTempoTrack(reader);
}

void
FollowerTypeIndependentImpl::StartNewBlock(std::pair<score_time_t, score_time_t> & scoreRange)
{
	auto currentBlock = timeManager_->GetRangeForNow();
	EnsureProperStart();
	ConsumeEvents();

	if (!rolling_) { return; }

	// Get start estimate based on old data
	scoreRange.first = timeWarper_->WarpTimestamp(currentBlock.first);

	// Fix the starting point, ensures the next warp is "accurate"
	speed_t speed = tempoFollower_->SpeedEstimateAt(scoreRange.first);
	timeWarper_->FixTimeMapping(currentBlock.first, scoreRange.first, speed);

	// Now use the new estimate for this block
	scoreRange.second = timeWarper_->WarpTimestamp(currentBlock.second);
}

unsigned
FollowerTypeIndependentImpl::ScoreTimeToFrameOffset(score_time_t const & time)
{
	real_time_t const & ref = timeManager_->CurrentBlockStart();
	real_time_t realTime = timeWarper_->InverseWarpTimestamp(time, ref);
	return timeManager_->ToSampleOffset(realTime);
}

void
FollowerTypeIndependentImpl::EnsureProperStart()
{
	if (started_) { return; }
	started_ = true;

	assert(eventProvider_->StartProduction());
}

void
FollowerTypeIndependentImpl::ConsumeEvents()
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
FollowerTypeIndependentImpl::ConsumeEvent(Event const & e)
{
	switch(e.type())
	{
	case Event::TrackingStarted:
		rolling_ = true;
		break;
	case Event::TrackingEnded:
		break;
	case Event::Beat:
		score_time_t warpedTime = timeWarper_->WarpTimestamp(e.timestamp());
		tempoFollower_->RegisterBeat(warpedTime);
		break;
	}
}

} // namespace ScoreFollower
} // namespace cf
