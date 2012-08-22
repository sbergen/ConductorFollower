#include "FollowerImpl.h"

#include <boost/make_shared.hpp>

#include "cf/globals.h"
#include "cf/math.h"
#include "Data/InstrumentParser.h"
#include "Data/ScoreParser.h"
#include "Data/BeatPatternParser.h"
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
	auto writer = status_.writer();
	eventThrottler_->ConsumeEventsUntil(
		[&, this](Event const & e)
		{
			ConsumeEvent(writer, e);
		},
		currentBlock.first);


	// Check for starting state
	// TODO intra-buffer start
	if (State() == FollowerState::GotStart &&
		currentBlock.first >= timeHelper_->StartTimeEstimate())
	{
		SetState(writer, FollowerState::Rolling);
	}

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

	case Event::VelocityPeak:
		writer->SetValue<Status::VelocityPeak>(e.data<double>());
		conductorContext_.velocity = math::clamp(
			e.data<double>() / Status::VelocityPeakType::max_value,
			0.0, 1.0);
		break;
	case Event::VelocityDynamicRange:
		writer->SetValue<Status::VelocityRange>(e.data<double>());
		conductorContext_.attack = math::clamp(
			e.data<double>() / Status::VelocityRangeType::max_value,
			0.3, 1.0);
		break;
	case Event::JerkPeak:
		writer->SetValue<Status::JerkPeak>(e.data<double>());
		conductorContext_.weight = math::clamp(
			e.data<double>() / Status::JerkPeakType::max_value,
			0.3, 1.0);
		break;
	case Event::Beat:
		if (State() == FollowerState::Rolling || State() == FollowerState::GotStart) {
			timeHelper_->RegisterBeat(e.timestamp(), e.data<double>());
		}
		break;
	case Event::BeatProb:
		writer->SetValue<Status::Beat>(e.data<double>());
		break;
	case Event::StartGesture:
		if (State() == FollowerState::WaitingForStart) {
			timeHelper_->RegisterStartGestureLength(e.data<duration_t>());
			SetState(writer, FollowerState::GotStart);
		}
		break;
	case Event::PreparatoryBeat:
		timeHelper_->RegisterPreparatoryBeat(e.data<real_time_t>());
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

	// Beat patterns
	Data::BeatPatternParser beatPatternParser;
	beatPatternParser.parse(scoreParser.data().beatPatternFile);
	timeHelper_->LearnPatterns(beatPatternParser.Patterns());

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
