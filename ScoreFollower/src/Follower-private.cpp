#include "ScoreFollower/Follower-private.h"

#include "FeatureExtractor/Event.h"
#include "FeatureExtractor/EventProvider.h"

#include "TimeWarper.h"
#include "AudioBlockTimeManager.h"

using namespace cf::FeatureExtractor;

namespace cf {
namespace ScoreFollower {

FollowerTypeIndependentImpl::FollowerTypeIndependentImpl(unsigned samplerate, unsigned blockSize)
	: started_(false)
	, rolling_(false)
{
	eventProvider_.reset(EventProvider::Create());
	timeManager_.reset(new AudioBlockTimeManager(samplerate, blockSize));
	timeWarper_.reset(new TimeWarper());
}

FollowerTypeIndependentImpl::~FollowerTypeIndependentImpl()
{
}

void
FollowerTypeIndependentImpl::ReadTempoTrack(TrackReader<tempo_t> & reader)
{
	timeWarper_->ReadTempoTrack(reader);
}

void
FollowerTypeIndependentImpl::StartNewBlock(std::pair<score_time_t, score_time_t> & scoreRange)
{
	auto currentBlock = timeManager_->GetRangeForNow();
	EnsureProperStart();
	ConsumeEvents();

	if (!rolling_) { return; }

	scoreRange.first = timeWarper_->WarpTimestamp(currentBlock.first);
	scoreRange.second = timeWarper_->WarpTimestamp(currentBlock.second);

	timeWarper_->FixTimeMapping(currentBlock.second, scoreRange.second);
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
	Event e;
	while (eventProvider_->DequeueEvent(e))
	{
		switch(e.type())
		{
		case Event::TrackingStarted:
			timeWarper_->FixTimeMapping(timeManager_->CurrentBlockStart(), score_time_t::zero());
			rolling_ = true;
			break;
		case Event::TrackingEnded:
			break;
		case Event::Beat:
			timeWarper_->RegisterBeat(e.timestamp());
			break;
		}
	}
}

} // namespace ScoreFollower
} // namespace cf
