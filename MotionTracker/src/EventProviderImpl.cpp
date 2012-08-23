#include "EventProviderImpl.h"

#include <boost/make_shared.hpp>

#include "cf/geometry.h"
#include "cf/globals.h"
#include "cf/RTContext.h"

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
		RTContext rt;
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
			// TODO don't run start gesture detection all the time
			DetectStartGesture(time, state, beatOccurred);
		});	
}

void
EventProviderImpl::RunMotionFilters(timestamp_t const & timeNow, MotionState const & state)
{
	// Ugly and temporary
	static Point3D prevPoint;

	Velocity3D::reduced<2>::type vel2D;
	state.velocity.reduceDimension(vel2D);

	auto const velocity = geometry::abs(vel2D).value();
	auto const velocityRange = velocityRange_.Run(velocity);
	auto const velFirOut = velocityFir_.Run(velocity);
	auto const velocityPeak = velocityPeakHolder_.Run(velFirOut);

	auto const velocityDev = velocityDev_.Run(geometry::abs(geometry::distance_vector(state.position, prevPoint)).value() * 100);

	eventBuffer_.enqueue(Event(timeNow, Event::VelocityDynamicRange, velocityDev));
	eventBuffer_.enqueue(Event(timeNow, Event::VelocityPeak, velocityPeak));

	//Acceleration3D::reduced<2>::type acc2D;
	//state.acceleration.reduceDimension(acc2D);

	Jerk3D::reduced<2>::type jerk2D;
	state.jerk.reduceDimension(jerk2D);

	auto const jerk = geometry::abs(jerk2D).value();
	auto const jerkFirOut = jerkFir_.Run(jerk);
	auto const jerkPeak = jerkPeakHolder_.Run(jerkFirOut);

	eventBuffer_.enqueue(Event(timeNow, Event::JerkPeak, jerkPeak));
	
	prevPoint = state.position;
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
		eventBuffer_.enqueue(
			Event(timeNow, Event::StartGesture,
				StartGestureData(startResult.previousBeatTime, startResult.gestureLength)));
	}
}

} // namespace MotionTracker
} // namespace cf
