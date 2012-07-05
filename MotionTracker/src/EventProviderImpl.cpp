#include "EventProviderImpl.h"

#include <boost/make_shared.hpp>

#include "MotionTracker/Event.h"

namespace cf {
namespace MotionTracker {

boost::shared_ptr<EventProvider>
EventProvider::Create()
{
	return boost::make_shared<EventProviderImpl>();
}

EventProviderImpl::EventProviderImpl()
	: eventBuffer_(1024)
{
	trackerThread_ = boost::make_shared<LockfreeThread>(
		boost::bind(&EventProviderImpl::Init, this),
		boost::bind(&EventProviderImpl::EventLoop, this),
		boost::bind(&EventProviderImpl::Cleanup, this));
}

EventProviderImpl::~EventProviderImpl()
{
	// Stop the thread before anything else gets destructed
	trackerThread_.reset();
}

bool EventProviderImpl::StartProduction()
{
	return trackerThread_->RequestStart();
}

bool EventProviderImpl::StopProduction()
{
	return trackerThread_->RequestStop();
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
	eventBuffer_.enqueue(Event(time::now(), Event::Position, pos));
}

bool
EventProviderImpl::Init()
{
	tracker_ = MotionTracker::HandTracker::Create();

	if (!tracker_->Init())
	{
		std::cout << "Failed to init!" << std::endl;
		return false;
	}

	return tracker_->StartTrackingHand(MotionTracker::GestureWave, *this);
}

bool
EventProviderImpl::EventLoop()
{
	return tracker_->WaitForData();
}

void
EventProviderImpl::Cleanup()
{
	tracker_.reset();
}

} // namespace MotionTracker
} // namespace cf
