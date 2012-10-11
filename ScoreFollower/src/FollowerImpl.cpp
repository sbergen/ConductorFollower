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
	, state_(status_, statusBuffer_)
{
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

	eventProvider_->Init();
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
	if (state_.ShouldDeliverEvents())
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
		contextWriter->rolling = (state_ == FollowerState::Rolling);
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
	if (!state_.ShouldDeliverEvents()) { return; }

	scoreHelper_->GetTrackEventsForBlock(track, events);
}

void
FollowerImpl::ConsumeEvent(Event const & e)
{
	switch(e.type())
	{
	case Event::TrackingStateChanged:
		state_.SetMotionTrackerState(e.data<TrackingState>());
		EnsureMotionTrackingIsStarted();
		break;
	case Event::HandStateChanged:
		state_.SetHandState(e.data<HandState>());
		break;
	case Event::MotionStateUpdate:
		// Not used
		break;
	case Event::VelocityPeak:
		if (state_.InPlayback()) { break; }
		status_.at<Status::VelocityPeak>() = e.data<double>();
		conductorContext_.velocity = math::clamp(
			e.data<double>() / Status::VelocityPeakType::max_value,
			0.0, 1.0);
		break;
	case Event::VelocityDynamicRange:
		if (state_.InPlayback()) { break; }
		status_.at<Status::VelocityRange>() = e.data<double>();
		conductorContext_.attack = math::clamp(
			e.data<double>() / Status::VelocityRangeType::max_value,
			0.3, 1.0);
		break;
	case Event::JerkPeak:
		if (state_.InPlayback()) { break; }
		status_.at<Status::JerkPeak>() = e.data<double>();
		conductorContext_.weight = math::clamp(
			e.data<double>() / Status::JerkPeakType::max_value,
			0.3, 1.0);
		break;
	case Event::Beat:
		if (state_ == FollowerState::Rolling) {
			auto beatEvent = timeHelper_->RegisterBeat(e.timestamp(), e.data<double>());
			statusEventProvider_.buffer_.enqueue(
				StatusEvent(e.timestamp(), StatusEvent::Beat, beatEvent));
		}
		break;
	case Event::BeatProb:
		status_.at<Status::Beat>() = e.data<double>();
		break;
	case Event::StartGesture:
		if (state_ == FollowerState::WaitingForStart) {
			timeHelper_->RegisterStartGesture(e.data<StartGestureData>());
			state_.SetRolling(true);
		}
		break;
	}
}

void
FollowerImpl::CheckForConfigChange()
{
	bool restart = optionsReader_->at<Options::Restart>().check();
	bool listen = optionsReader_->at<Options::Listen>().check();
	std::string scoreFile = optionsReader_->at<Options::ScoreDefinition>();

	// If no action is required, return
	if (!(listen || restart || (scoreFile != scoreFile_)) ||
		scoreFile == "") {
		return;
	}

	// Reset score file comparison
	scoreFile_ = scoreFile;

	Lock lock(configMutex_);

	timeHelper_ = timeHelper_->FreshClone();
	scoreHelper_->ResetTimeHelper(timeHelper_);

	if (listen) {
		conductorContext_ = PatchMapper::ConductorContext();
		timeHelper_->StartAtDefaultTempo();
	}
	
	CollectData(scoreFile);
	EnsureMotionTrackingIsStarted();

	state_.SetRolling(false);
	state_.SetPlayback(listen);
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
FollowerImpl::EnsureMotionTrackingIsStarted()
{
	if (state_.TrackingStopped()) {
		eventProvider_->StartProduction();
	}
}

FollowerImpl::State::State(Status::FollowerStatus & status, StatusBuffer & statusBuffer)
	: status_(status)
	, statusBuffer_(statusBuffer)
	, trackingState_(MotionTracker::TrackingStopped)
	, handState_(MotionTracker::HandState::Lost)
	, rolling_(false)
	, playback_(false)
{
	status_.at<Status::State>() = FollowerState::Stopped;
}

void
FollowerImpl::State::SetMotionTrackerState(MotionTracker::TrackingState trackingState, bool propagateChange)
{
	trackingState_ = trackingState;

	// Stop everything on all tracker events (this is a feature)
	playback_ = false;
	rolling_ = false;

	UpdateState(propagateChange);
}

void
FollowerImpl::State::SetHandState(MotionTracker::HandState handState, bool propagateChange)
{
	handState_ = handState.state;

	// Stop everything on all hand events (this is a feature)
	playback_ = false;
	rolling_ = false;

	UpdateState(propagateChange);
}

void
FollowerImpl::State::SetRolling(bool rolling, bool propagateChange)
{
	rolling_ = rolling;
	UpdateState(propagateChange);
}

void
FollowerImpl::State::SetPlayback(bool playback, bool propagateChange)
{
	playback_ = playback;
	UpdateState(propagateChange);
}

void
FollowerImpl::State::UpdateState(bool propagateChange)
{
	status_.at<Status::State>() = ResolveState();
	if (propagateChange)
	{
		auto writer = statusBuffer_.GetWriter();
		*writer = status_;
	}
}

FollowerState::Value
FollowerImpl::State::ResolveState()
{
	if (playback_) {
		return FollowerState::Playback;
	}

	if (rolling_) {
		return FollowerState::Rolling;
	}

	switch (trackingState_)
	{
	case MotionTracker::TrackingStopped:
		return FollowerState::Stopped;
	case MotionTracker::TrackingStarting:
		return FollowerState::StartingUp;
	case MotionTracker::TrackingOnline:
		// Chceck hand state
		break;
	}

	switch (handState_)
	{
	case MotionTracker::HandState::Lost:
		return FollowerState::WaitingForCalibration;
	case MotionTracker::HandState::Found:
		return FollowerState::WaitingForStart;
	}

	throw std::runtime_error("Logic error in follower state handling code!");
}

} // namespace ScoreFollower
} // namespace cf
