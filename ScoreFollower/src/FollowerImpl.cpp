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
	: optionsReader_(optionsBuffer_)
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
	optionsReader_->GetValue<Options::Restart>(restartVersion_);
	optionsReader_->GetValue<Options::ScoreDefinition>(scoreFile_);

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
	eventThrottler_->ConsumeEventsUntil(
		[&, this](Event const & e)
		{
			ConsumeEvent(e);
		},
		currentBlock.first);


	// Check for starting state
	if (State() == FollowerState::GotStart &&
		currentBlock.second >= timeHelper_->StartTimeEstimate())
	{
		SetState(FollowerState::Rolling, false);
	}

	unsigned ret = 0;
	if (State() == FollowerState::Rolling ||
		State() == FollowerState::GotStart)
	{
		// If rolling, fix score range
		timeHelper_->FixScoreRange(status_);
		ret = scoreReader_->TrackCount();
	}

	auto writer = statusBuffer_.GetWriter();
	*writer = status_;

	return ret;
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
	FollowerState state;
	status_.GetValue<Status::State>(state);
	return state;
}

void
FollowerImpl::SetState(FollowerState::Value state, bool propagateChange)
{
	status_.SetValue<Status::State>(state);
	if (propagateChange)
	{
		auto writer = statusBuffer_.GetWriter();
		*writer = status_;
	}
}

void
FollowerImpl::ConsumeEvent(Event const & e)
{
	switch(e.type())
	{
	case Event::TrackingStarted:
		SetState(FollowerState::WaitingForStart, false);
		break;
	case Event::TrackingEnded:
		SetState(FollowerState::Stopped, false);
		eventProvider_->StopProduction();
		break;
	case Event::MotionStateUpdate:
		// Not used
		break;

	case Event::VelocityPeak:
		status_.SetValue<Status::VelocityPeak>(e.data<double>());
		conductorContext_.velocity = math::clamp(
			e.data<double>() / Status::VelocityPeakType::max_value,
			0.0, 1.0);
		break;
	case Event::VelocityDynamicRange:
		status_.SetValue<Status::VelocityRange>(e.data<double>());
		conductorContext_.attack = math::clamp(
			e.data<double>() / Status::VelocityRangeType::max_value,
			0.3, 1.0);
		break;
	case Event::JerkPeak:
		status_.SetValue<Status::JerkPeak>(e.data<double>());
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
		status_.SetValue<Status::Beat>(e.data<double>());
		break;
	case Event::StartGesture:
		if (State() == FollowerState::WaitingForStart) {
			timeHelper_->RegisterStartGesture(e.data<StartGestureData>());
			SetState(FollowerState::GotStart, false);
		}
		break;
	case Event::VisualizationData:
		visualizationData_ = e.data<Visualizer::DataPtr>();
	}
}

void
FollowerImpl::CheckForConfigChange()
{
	// Restart
	int restart;
	bool forceScoreRead = false;
	optionsReader_->GetValue<Options::Restart>(restart);
	if (restart != restartVersion_) {
		restartVersion_ = restart;
		forceScoreRead = true;
		RestartScore();
	}

	// Score file
	std::string scoreFile;
	optionsReader_->GetValue<Options::ScoreDefinition>(scoreFile);

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

	// Events
	timeHelper_->LearnScoreEvents(scoreParser.data().events);

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
