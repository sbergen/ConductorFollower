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
		, tracker_(parent.tracker_)
	{
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
	: musicalContextReader_(musicalContextBuffer_)
	, startDetector_(musicalContextReader_)
	, visualizationData_(boost::make_shared<Visualizer::Data>())
	, visualizationBuffer_(boost::make_shared<Visualizer::DataBuffer>())
{
}

EventProviderImpl::~EventProviderImpl()
{
	// Stop the thread before anything else gets destructed
	trackerThread_.reset();
}

void EventProviderImpl::StartProduction()
{
	if (!trackerThread_) {
		tracker_ = MotionTracker::HandTracker::Create();

		tracker_->AddTrackingStateObserver(*this);

		if (!tracker_->Init())
		{
			GlobalsRef globals;
			globals.ErrorBuffer()->enqueue("Failed to init Motion tracker!");
			return;
		}

		tracker_->AddVisualizationObserver(*this);

		auto factory = boost::bind(boost::make_shared<TrackerThread, EventProviderImpl &>, boost::ref(*this));
		trackerThread_ = boost::make_shared<LockfreeThread<TrackerThread> >(
			factory, *globalsRef_.Butler());
	}

	trackerThread_->RequestStart();
}

void EventProviderImpl::StopProduction()
{
	if (!trackerThread_) { return; }

	trackerThread_->RequestStop();
}

boost::shared_ptr<EventQueue>
EventProviderImpl::GetEventQueue()
{
	boost::mutex::scoped_lock lock(queueMutex_);

	// First clean up old ones, then create new
	CleanUpQueues();

	auto queue = boost::make_shared<Queue>();
	queues_.push_back(queue);
	return queue;
}

MusicalContextBuffer::Writer
EventProviderImpl::GetMusicalContextWriter()
{
	return musicalContextBuffer_.GetWriter();
}

void
EventProviderImpl::HandFound()
{
	HandState state = { HandState::Right, HandState::Found };
	QueueEvent(Event(time::now(), Event::HandStateChanged, state));
}

void
EventProviderImpl::HandLost()
{
	HandState state = { HandState::Right, HandState::Lost };
	QueueEvent(Event(time::now(), Event::HandStateChanged, state));

	// Restart automatically, this is in a callback,
	// so it should be thread-safe
	tracker_->StartTrackingHand(MotionTracker::GestureWave, *this);
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
EventProviderImpl::InitVisualizationData(int width, int height)
{
	visualizationData_->Reserve(width, height);
	visualizationBuffer_->Init(
		[width, height](Visualizer::Data & data)
		{
			data.Reserve(width, height);
		});
}

Visualizer::DataPtr
EventProviderImpl::GetVisualizationData()
{
	return visualizationData_;
}

void
EventProviderImpl::NewVisualizationData()
{
	{
		auto writer = visualizationBuffer_->GetWriter();
		*writer = *visualizationData_;
	}
	QueueEvent(Event(time::now(), Event::VisualizationData, visualizationBuffer_));
}

void
EventProviderImpl::NewVisualizationHandPosition(Visualizer::Position const & pos)
{
	QueueEvent(Event(time::now(), Event::VisualizationHandPosition, pos));
}

void
EventProviderImpl::TrackingStateChanged(TrackingState const & newState)
{
	QueueEvent(Event(time::now(), Event::TrackingStateChanged, newState));
}

void
EventProviderImpl::QueueEvent(Event const & e)
{
	boost::mutex::scoped_try_lock lock(queueMutex_);

	if (lock.owns_lock()) {
		for (auto it = queues_.begin(); it != queues_.end(); ++it) {
			(*it)->eventBuffer.enqueue(e);
		}
	} else {
		LOG("!!! Missing an event because of queue locking!");
	}
}

void
EventProviderImpl::CleanUpQueues()
{
	for (auto it = queues_.begin(); it != queues_.end(); ++it) {
		if (it->use_count() == 1) {
			it->reset();
		}
	}
	auto newEnd = std::remove(std::begin(queues_), std::end(queues_),
		boost::shared_ptr<Queue>()); // "null"
	queues_.erase(newEnd, std::end(queues_));
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

	QueueEvent(Event(timeNow, Event::VelocityDynamicRange, velocityDev));
	QueueEvent(Event(timeNow, Event::VelocityPeak, velocityPeak));

	//Acceleration3D::reduced<2>::type acc2D;
	//state.acceleration.reduceDimension(acc2D);

	Jerk3D::reduced<2>::type jerk2D;
	state.jerk.reduceDimension(jerk2D);

	auto const jerk = geometry::abs(jerk2D).value();
	auto const jerkFirOut = jerkFir_.Run(jerk);
	auto const jerkPeak = jerkPeakHolder_.Run(jerkFirOut);

	QueueEvent(Event(timeNow, Event::JerkPeak, jerkPeak));
	
	prevPoint = state.position;
}

bool
EventProviderImpl::DetectBeat(timestamp_t const & timeNow, MotionState const & state)
{
	double beatVal;
	bool beat = beatDetector_.Detect(state, beatVal);
	QueueEvent(Event(timeNow, Event::BeatProb, beatVal));
	if (beat) {
		QueueEvent(Event(timeNow, Event::Beat, beatVal));
	}
	
	return beat;
}

void
EventProviderImpl::DetectStartGesture(timestamp_t const & timeNow, MotionState const & state, bool beatOccurred)
{
	auto startResult = startDetector_.Detect(timeNow, state, beatOccurred);
	if (startResult) {
		QueueEvent(
			Event(timeNow, Event::StartGesture,
				StartGestureData(startResult.previousBeatTime, startResult.gestureLength)));
	}
}

} // namespace MotionTracker
} // namespace cf
