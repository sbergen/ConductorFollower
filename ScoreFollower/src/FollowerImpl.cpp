#include "FollowerImpl.h"

#include <boost/make_shared.hpp>
#include <cmath>

#include "cf/globals.h"
#include "cf/math.h"
#include "Data/InstrumentParser.h"
#include "Data/ScoreParser.h"
#include "Data/BeatPatternParser.h"
#include "Data/ParseException.h"
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
	eventThrottler_ = boost::make_shared<EventThrottler>(eventProvider_->GetEventQueue());
	scoreHelper_ = boost::make_shared<ScoreHelper>(timeHelper_, conductorContext_);

	// Change tracking
	scoreFile_ = optionsReader_->at<Options::ScoreDefinition>();

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

	unsigned ret = 0;
	if (State() == FollowerState::Rolling)
	{
		// If rolling, fix score range
		timeHelper_->FixScoreRange(status_);
		ret = scoreReader_->TrackCount();
	}

	// Update stati

	{
		auto statusWriter = statusBuffer_.GetWriter();
		*statusWriter = status_;
	}

	{
		auto tempoInfo = timeHelper_->CurrentTempoInfo();
		auto contextWriter = eventProvider_->GetMusicalContextWriter();
		
		contextWriter->currentTempo = tempoInfo.current;
		contextWriter->scoreTempo = tempoInfo.score;
		contextWriter->rolling = (State() == FollowerState::Rolling);
	}

	if (ret > 0) {
		// Use block end, as the visualization will have some lag
		auto realTime = currentBlock.second;
		auto scorePos = timeHelper_->ScorePositionAt(realTime);
		double phase = scorePos.FractionOfBar();
		assert(phase >= 0.0 && phase <= 1.0);
		statusEventProvider_.buffer_.enqueue(
			StatusEvent(realTime, StatusEvent::BarPhase, BarPhaseEvent(phase)));
	}

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
	FollowerState state = status_.at<Status::State>();
	return state;
}

void
FollowerImpl::SetState(FollowerState::Value state, bool propagateChange)
{
	status_.at<Status::State>() = state;
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
	case Event::TrackingStateChanged:
		HandleTrackingStateChange(e.data<TrackingState>());
		break;
	case Event::HandStateChanged:
		HandleHandStateChange(e.data<HandState>());
		break;
	case Event::MotionStateUpdate:
		// Not used
		break;
	case Event::VelocityPeak:
		status_.at<Status::VelocityPeak>() = e.data<double>();
		conductorContext_.velocity = math::clamp(
			e.data<double>() / Status::VelocityPeakType::max_value,
			0.0, 1.0);
		break;
	case Event::VelocityDynamicRange:
		status_.at<Status::VelocityRange>() = e.data<double>();
		conductorContext_.attack = math::clamp(
			e.data<double>() / Status::VelocityRangeType::max_value,
			0.3, 1.0);
		break;
	case Event::JerkPeak:
		status_.at<Status::JerkPeak>() = e.data<double>();
		conductorContext_.weight = math::clamp(
			e.data<double>() / Status::JerkPeakType::max_value,
			0.3, 1.0);
		break;
	case Event::Beat:
		if (State() == FollowerState::Rolling) {
			auto beatEvent = timeHelper_->RegisterBeat(e.timestamp(), e.data<double>());
			statusEventProvider_.buffer_.enqueue(
				StatusEvent(e.timestamp(), StatusEvent::Beat, beatEvent));
		}
		break;
	case Event::BeatProb:
		status_.at<Status::Beat>() = e.data<double>();
		break;
	case Event::StartGesture:
		if (State() == FollowerState::WaitingForStart) {
			timeHelper_->RegisterStartGesture(e.data<StartGestureData>());
			SetState(FollowerState::Rolling);
		}
		break;
	}
}

void
FollowerImpl::HandleTrackingStateChange(MotionTracker::TrackingState const & state)
{
	switch(state)
	{
		case TrackingStarting:
			SetState(FollowerState::StartingUp, false);
			break;
		case TrackingOnline:
			SetState(FollowerState::WaitingForCalibration, false);
			break;
		case TrackingStopped:
			SetState(FollowerState::WaitingForCalibration, false);
			EnsureMotionTrackingIsStarted();
			break;
	}
}

void
FollowerImpl::HandleHandStateChange(MotionTracker::HandState const & state)
{
	switch(state.state)
	{
	case HandState::Found:
		SetState(FollowerState::WaitingForStart, false);
		break;
	case HandState::Lost:
		SetState(FollowerState::WaitingForCalibration, false);
		break;
	}
}

void
FollowerImpl::CheckForConfigChange()
{
	// Restart
	bool forceScoreRead = false;
	auto & restart = optionsReader_->at<Options::Restart>();
	if (restart.check()) {
		forceScoreRead = true;
		RestartScore();
	}

	// "Preview"
	auto & listen = optionsReader_->at<Options::Listen>();
	if (listen.check() && !scoreFile_.empty()) {
		ListenToScore();
	}

	// Score file
	std::string scoreFile = optionsReader_->at<Options::ScoreDefinition>();

	if ((forceScoreRead ||scoreFile != scoreFile_) && scoreFile != "") {
		scoreFile_ = scoreFile;
		CollectData(scoreFile);
	}
}

void
FollowerImpl::CollectData(std::string const & scoreFile)
{
	try {

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

	} catch(Data::ParseException err) {
		globalsRef_.ErrorBuffer()->enqueue(err.what());
		return;
	}

	EnsureMotionTrackingIsStarted();
}

void
FollowerImpl::RestartScore()
{
	Lock lock(configMutex_);
	timeHelper_ = timeHelper_->FreshClone();
	scoreHelper_->ResetTimeHelper(timeHelper_);
	EnsureMotionTrackingIsStarted();
}

void
FollowerImpl::ListenToScore()
{
	Lock lock(configMutex_);
	// TODO, if rolling, do something here
	//timeHelper_ = timeHelper_->FreshClone();
	scoreHelper_->ResetTimeHelper(timeHelper_);
	timeHelper_->StartAtDefaultTempo();
	SetState(FollowerState::Rolling);
}

void
FollowerImpl::EnsureMotionTrackingIsStarted()
{
	switch(State())
	{
	case FollowerState::StartingUp:
	case FollowerState::WaitingForCalibration:
	case FollowerState::WaitingForStart:
		break;
	case FollowerState::Rolling:
		SetState(FollowerState::WaitingForStart);
		break;
	case FollowerState::Stopped:
		eventProvider_->StartProduction();
		break;
	}
}

} // namespace ScoreFollower
} // namespace cf
