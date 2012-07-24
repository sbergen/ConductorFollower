#include "EventProviderImpl.h"

#include <boost/make_shared.hpp>

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

	motionFilter_.NewPosition(realTime, pos);
	eventBuffer_.enqueue(Event(realTime, Event::MotionStateUpdate, motionFilter_.State()));

	/*timestamp_t beatTime;
	if (beatDetector_.NewState(realTime, motionFilter_.State(), beatTime))
	{
		eventBuffer_.enqueue(Event(beatTime, Event::Beat));
	}*/
	auto beatVal = beatDetector_.ValFromState(motionFilter_.State());
	eventBuffer_.enqueue(Event(realTime, Event::BeatProb, beatVal));
	if (beatVal > 0.0) {
		eventBuffer_.enqueue(Event(realTime, Event::Beat));
	}
}

} // namespace MotionTracker
} // namespace cf
