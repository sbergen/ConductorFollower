#include "EventProviderImpl.h"

#include <boost/make_shared.hpp>

#include "cf/geometry.h"
#include "cf/globals.h"

#include "MotionTracker/Event.h"

namespace cf {
namespace MotionTracker {

// factory function
boost::shared_ptr<EventProvider>
EventProvider::Create()
{
	return boost::make_shared<EventProviderImpl>();
}

class EventProviderImpl::TrackerThread
{
public:
	TrackerThread(EventProviderImpl & parent)
		: parent_(parent)
	{
		tracker_ = MotionTracker::HandTracker::Create();

		if (!tracker_->Init())
		{
			std::cout << "Failed to init!" << std::endl;
			tracker_.reset();
			return;
		}

		tracker_->StartTrackingHand(MotionTracker::GestureWave, parent);
	}

	bool operator() ()
	{
		return tracker_->WaitForData();
	}

private:
	EventProviderImpl & parent_;
	boost::shared_ptr<HandTracker> tracker_;
};


EventProviderImpl::EventProviderImpl()
	: eventBuffer_(1024)
{
	auto factory = boost::bind(boost::make_shared<TrackerThread, EventProviderImpl &>, boost::ref(*this));
	trackerThread_ = boost::make_shared<LockfreeThread<TrackerThread> >(
		factory, *globalsRef_.Butler());
}

EventProviderImpl::~EventProviderImpl()
{
	// Stop the thread before anything else gets destructed
	trackerThread_.reset();
}

void EventProviderImpl::StartProduction()
{
	trackerThread_->RequestStart();
}

void EventProviderImpl::StopProduction()
{
	trackerThread_->RequestStop();
}

bool EventProviderImpl::DequeueEvent(Event & result)
{
	return eventBuffer_.dequeue(result);
}

void
EventProviderImpl::HandFound()
{
	eventBuffer_.enqueue(Event(time::now(), Event::TrackingStarted));
}

void
EventProviderImpl::HandLost()
{
	eventBuffer_.enqueue(Event(time::now(), Event::TrackingEnded));
}

void
EventProviderImpl::NewHandPosition(float time, Point3D const & pos)
{
	auto realTime = time::now();

	// Motion filter
	motionFilter_.NewPosition(realTime, pos,
		[this](timestamp_t const & time, MotionState const & state)
		{
			RunMotionFilters(time, state);
			bool beatOccurred = DetectBeat(time, state);
			DetectStartGesture(time, state, beatOccurred);
		});	
}

void
EventProviderImpl::RunMotionFilters(timestamp_t const & timeNow, MotionState const & state)
{
	auto const velocity = geometry::abs(state.velocity).value();
	auto const velocityRange = velocityRange_.Run(velocity);
	auto const velFirOut = velocityFir_.Run(velocity);
	auto const velocityPeak = velocityPeakHolder_.Run(velFirOut);

	eventBuffer_.enqueue(Event(timeNow, Event::VelocityDynamicRange, velocityRange));
	eventBuffer_.enqueue(Event(timeNow, Event::VelocityPeak, velocityPeak));

	auto const jerk = geometry::abs(state.jerk).value();
	auto const jerkFirOut = jerkFir_.Run(jerk);
	auto const jerkPeak = jerkPeakHolder_.Run(jerkFirOut);

	eventBuffer_.enqueue(Event(timeNow, Event::JerkPeak, jerkPeak));
}

bool
EventProviderImpl::DetectBeat(timestamp_t const & timeNow, MotionState const & state)
{
	double beatVal;
	bool beat = beatDetector_.Detect(state, beatVal);
	eventBuffer_.enqueue(Event(timeNow, Event::BeatProb, beatVal));
	if (beat) {
		eventBuffer_.enqueue(Event(timeNow, Event::Beat, beatVal));
	}
	return beat;
}

void
EventProviderImpl::DetectStartGesture(timestamp_t const & timeNow, MotionState const & state, bool beatOccurred)
{
	auto startResult = startDetector_.Detect(timeNow, state, beatOccurred);
	if (startResult) {
		eventBuffer_.enqueue(Event(timeNow, Event::StartGesture, startResult.previousBeatTime));
	}
}

} // namespace MotionTracker
} // namespace cf
