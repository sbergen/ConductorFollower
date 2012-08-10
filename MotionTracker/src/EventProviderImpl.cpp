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
	if (!motionFilter_.NewPosition(realTime, pos)) { return; }
	//eventBuffer_.enqueue(Event(realTime, Event::MotionStateUpdate, motionFilter_.State()));

	// Calculations
	RunMotionFilters(realTime);
	bool beatOccurred = DetectBeat(realTime);
	DetectStartGesture(realTime, beatOccurred);
}

void
EventProviderImpl::RunMotionFilters(timestamp_t const & timeNow)
{
	auto const velocity = geometry::abs(motionFilter_.State().velocity).value();
	auto const velocityRange = velocityRange_.Run(velocity);
	auto const velFirOut = velocityFir_.Run(velocity);
	auto const velocityPeak = velocityPeakHolder_.Run(velFirOut);

	eventBuffer_.enqueue(Event(timeNow, Event::VelocityDynamicRange, velocityRange));
	eventBuffer_.enqueue(Event(timeNow, Event::VelocityPeak, velocityPeak));

	auto const jerk = geometry::abs(motionFilter_.State().jerk).value();
	auto const jerkFirOut = jerkFir_.Run(jerk);
	auto const jerkPeak = jerkPeakHolder_.Run(jerkFirOut);

	eventBuffer_.enqueue(Event(timeNow, Event::JerkPeak, jerkPeak));
}

bool
EventProviderImpl::DetectBeat(timestamp_t const & timeNow)
{
	double beatVal;
	bool beat = beatDetector_.Detect(motionFilter_.State(), beatVal);
	eventBuffer_.enqueue(Event(timeNow, Event::BeatProb, beatVal));
	if (beat) {
		eventBuffer_.enqueue(Event(timeNow, Event::Beat, beatVal));
	}
	return beat;
}

void
EventProviderImpl::DetectStartGesture(timestamp_t const & timeNow, bool beatOccurred)
{
	auto startResult = startDetector_.Detect(timeNow, motionFilter_.State(), beatOccurred);
	if (startResult) {
		eventBuffer_.enqueue(Event(timeNow, Event::StartGesture, startResult.previousBeatTime));
	}
}

} // namespace MotionTracker
} // namespace cf
