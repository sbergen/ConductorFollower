#include "FollowerImpl.h"

#include <boost/make_shared.hpp>

#include "cf/globals.h"
#include "Data/InstrumentParser.h"
#include "Data/ScoreParser.h"
#include "MotionTracker/EventProvider.h"
#include "MotionTracker/EventThrottler.h"
#include "FeatureExtractor/Extractor.h"

#include "ScoreFollower/ScoreEvent.h"

#include "TimeWarper.h"
#include "TempoFollower.h"
#include "AudioBlockTimeManager.h"

using namespace cf::FeatureExtractor;
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
	: status_(boost::make_shared<Status::FollowerStatus>())
	, options_(boost::make_shared<Options::FollowerOptions>())
	, startRollingTime_(real_time_t::max())
	, scoreReader_(scoreReader)
{
	// Construct memebers
	timeHelper_ = boost::make_shared<TimeHelper>(*this);
	eventProvider_= EventProvider::Create();
	eventThrottler_ = boost::make_shared<EventThrottler>(*eventProvider_);
	featureExtractor_ = Extractor::Create();
	scoreHelper_ = boost::make_shared<ScoreHelper>(timeHelper_);

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

	// Consume events until the start of this block
	eventThrottler_->ConsumeEventsUntil(
		boost::bind(&FollowerImpl::ConsumeEvent, this, _1),
		currentBlock.first);

	// Lock config before using timeHelper
	TryLock lock(configMutex_);
	if (!lock.owns_lock()) { return 0; }

	// Check state
	if (State() == FollowerState::WaitingForStart && currentBlock.first >= startRollingTime_) {
		SetState(FollowerState::Rolling);
		beatConnection_ = featureExtractor_->BeatDetected.connect(
			boost::bind(&TimeHelper::RegisterBeat, timeHelper_, _1));
	}

	if (State() != FollowerState::Rolling) { return 0; }

	// If rolling, fix score range
	timeHelper_->FixScoreRange();
	return scoreReader_->TrackCount();
}

void
FollowerImpl::GetTrackEventsForBlock(unsigned track, BlockBuffer & events)
{
	events.Clear();
	
	if (State() != FollowerState::Rolling) { return; }

	// Lock config and get data
	TryLock lock(configMutex_);
	if (!lock.owns_lock()) { return; }
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
	state_ = state;
	status_.write()->SetValue<Status::State>(state);
}


void
FollowerImpl::GotStartGesture(real_time_t const & beatTime, real_time_t const & startTime)
{
	startGestureConnection_.disconnect();

	timeHelper_->RegisterBeat(beatTime);
	startRollingTime_ = startTime;
}

void
FollowerImpl::ConsumeEvent(Event const & e)
{
	switch(e.type())
	{
	case Event::TrackingStarted:
		SetState(FollowerState::WaitingForStart);
		break;
	case Event::TrackingEnded:
		SetState(FollowerState::Stopped);
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
	UpdateMagnitude(timestamp);
}

void
FollowerImpl::UpdateMagnitude(real_time_t const & timestamp)
{
	// Make better
	Point3D distance = featureExtractor_->MagnitudeOfMovementSince(timestamp - milliseconds_t(1500));
	coord_t magnitude = geometry::abs(distance);
	status_.write()->SetValue<Status::MagnitudeOfMovement>(magnitude.value() * 10); // TODO
	scoreHelper_->SetVelocityFromMotion(magnitude / coord_t(60 * si::centi * si::meters));
}

void
FollowerImpl::CheckForConfigChange()
{
	static std::string scoreFile;

	auto options = options_.read();
	bool somethingChanged = options->LoadIfChanged<Options::ScoreDefinition>(scoreFile);

	if (somethingChanged && scoreFile != "")
	{
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

	// Start listening to gestures
	SetState(FollowerState::WaitingForCalibration);
	startGestureConnection_ = featureExtractor_->StartGestureDetected.connect(
		boost::bind(&FollowerImpl::GotStartGesture, this, _1, _2));
	eventProvider_->StartProduction();
}

} // namespace ScoreFollower
} // namespace cf
