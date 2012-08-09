#include "FollowerImpl.h"

#include <boost/make_shared.hpp>

#include "cf/globals.h"
#include "Data/InstrumentParser.h"
#include "Data/ScoreParser.h"
#include "MotionTracker/EventProvider.h"
#include "MotionTracker/EventThrottler.h"

#include "ScoreFollower/ScoreEvent.h"

#include "ScoreHelper.h"
#include "TimeWarper.h"
#include "TimeHelper.h"
#include "TempoFollower.h"
#include "AudioBlockTimeManager.h"

using namespace cf::MotionTracker;

namespace cf {
namespace ScoreFollower {

namespace si = boost::units::si;

boost::shared_ptr<Follower>
Follower::Create(boost::shared_ptr<ScoreReader> scoreReader)
{
	return boost::make_shared<FollowerImpl>(scoreReader);
}

FollowerImpl::FollowerImpl(boost::shared_ptr<ScoreReader> scoreReader)
	: status_(Status::FollowerStatus())
	, options_(Options::FollowerOptions())
	, scoreReader_(scoreReader)
{
	// Set proper initial state
	SetState(FollowerState::Stopped);

	// Construct memebers
	timeHelper_ = boost::make_shared<TimeHelper>(*this, conductorContext_);
	eventProvider_= EventProvider::Create();
	eventThrottler_ = boost::make_shared<EventThrottler>(*eventProvider_);
	scoreHelper_ = boost::make_shared<ScoreHelper>(timeHelper_, conductorContext_);

	// Change tracking
	options_.read()->GetValue<Options::Restart>(restartVersion_);
	options_.read()->GetValue<Options::ScoreDefinition>(scoreFile_);

	// Hook up to butler thread
	configCallbackHandle_ = globalsRef_.Butler()->AddCallback(
		boost::bind(&FollowerImpl::CheckForConfigChange, this));
}

FollowerImpl::~FollowerImpl()
{
}

void
FollowerImpl::SetBlockParameters(unsigned samplerate, unsigned blockSize)
{
	timeHelper_->SetBlockParameters(samplerate, blockSize);
}

unsigned
FollowerImpl::StartNewBlock()
{
	// Start new RT block
	timeHelper_->StartNewBlock();
	auto const & currentBlock = timeHelper_->CurrentRealTimeBlock();

	// Lock config before going further
	TryLock lock(configMutex_);
	if (!lock.owns_lock()) { return 0; }

	// Consume events until the start of this block
	// TODO bind
	auto writer = status_.writer();
	eventThrottler_->ConsumeEventsUntil(
		[&, this](Event const & e)
		{
			ConsumeEvent(writer, e);
		},
		currentBlock.first);

	if (State() != FollowerState::Rolling) { return 0; }

	// If rolling, fix score range
	timeHelper_->FixScoreRange(writer);
	return scoreReader_->TrackCount();
}

void
FollowerImpl::GetTrackEventsForBlock(unsigned track, BlockBuffer & events)
{
	events.Clear();
	TryLock lock(configMutex_);
	if (!lock.owns_lock()) { return; }
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
	SetState(status_.writer(), state);
}

void
FollowerImpl::SetState(StatusRCU::WriterHandle & writer, FollowerState::Value state)
{
	state_ = state;
	writer->SetValue<Status::State>(state);
}

void
FollowerImpl::ConsumeEvent(StatusRCU::WriterHandle & writer, Event const & e)
{
	switch(e.type())
	{
	case Event::TrackingStarted:
		SetState(writer, FollowerState::WaitingForStart);
		break;
	case Event::TrackingEnded:
		SetState(writer, FollowerState::Stopped);
		eventProvider_->StopProduction();
		break;
	case Event::MotionStateUpdate:
		// Not used
		break;
	case Event::Power:
		{
		double power = e.data<double>() / 1000.0;
		if (power > 1.0) { power = 1.0; }
		conductorContext_.power = power;
		writer->SetValue<Status::Power>(power);
		scoreHelper_->SetVelocityFromMotion(power);
		break;
		}
	case Event::Beat:
		{
		if (State() == FollowerState::GotStart) {
			SetState(writer, FollowerState::Rolling);
		}

		if (State() == FollowerState::Rolling) {
			timeHelper_->RegisterBeat(e.timestamp(), e.data<double>());
		}
		break;
		}
	case Event::BeatProb:
		writer->SetValue<Status::Beat>(e.data<double>());
		break;
	case Event::StartGesture:
		if (State() == FollowerState::WaitingForStart) {
			timeHelper_->RegisterPreparatoryBeat(e.data<real_time_t>());
			SetState(writer, FollowerState::GotStart);
		}
		break;
	}
}

void
FollowerImpl::CheckForConfigChange()
{
	// Restart
	int restart;
	bool forceScoreRead = false;
	options_.read()->GetValue<Options::Restart>(restart);
	if (restart != restartVersion_) {
		restartVersion_ = restart;
		forceScoreRead = true;
		RestartScore();
	}

	// Score file
	std::string scoreFile;
	auto options = options_.read();
	options->GetValue<Options::ScoreDefinition>(scoreFile);

	if ((forceScoreRead ||scoreFile != scoreFile_) && scoreFile != "") {
		scoreFile_ = scoreFile;
		CollectData(scoreFile);
	}
}

void
FollowerImpl::CollectData(std::string const & scoreFile)
{
	// TODO error handling

	// Parse score
	Data::ScoreParser scoreParser;
	scoreParser.parse(scoreFile);

	// Parse instruments
	Data::InstrumentParser instrumentParser;
	instrumentParser.parse(scoreParser.data().instrumentFile);

	// lock
	Lock lock(configMutex_);

	// Score
	scoreReader_->OpenFile(scoreParser.data().midiFile);
	timeHelper_->ReadScore(*scoreReader_);
	scoreHelper_->LearnScore(scoreReader_);
	
	// Instrument mappings
	scoreHelper_->LearnInstruments(instrumentParser.Instruments(), scoreParser.data().tracks);

	EnsureMotionTrackingIsStarted();
}

void
FollowerImpl::RestartScore()
{
	// Lock
	Lock lock(configMutex_);
	timeHelper_ = timeHelper_->FreshClone();
	scoreHelper_->ResetTimeHelper(timeHelper_);
	EnsureMotionTrackingIsStarted();
}

void
FollowerImpl::EnsureMotionTrackingIsStarted()
{
	switch(State())
	{
	case FollowerState::WaitingForCalibration:
	case FollowerState::WaitingForStart:
		break;
	case FollowerState::GotStart:
	case FollowerState::Rolling:
		SetState(FollowerState::WaitingForStart);
		break;
	case FollowerState::Stopped:
		SetState(FollowerState::WaitingForCalibration);
		eventProvider_->StartProduction();
		break;
	}
}

} // namespace ScoreFollower
} // namespace cf
