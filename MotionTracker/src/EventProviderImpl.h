#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/lockfree/ringbuffer.hpp>
#include <boost/thread.hpp>

#include "cf/LockfreeThread.h"
#include "cf/time.h"
#include "cf/geometry.h"
#include "cf/globals.h"
#include "cf/fir.h"
#include "cf/EMA.h"
#include "cf/DynamicRange.h"
#include "cf/PeakHolder.h"
#include "cf/StdDev.h"

#include "MotionTracker/EventProvider.h"
#include "MotionTracker/EventQueue.h"

#include "common_types.h"
#include "HandObserver.h"
#include "HandTracker.h"
#include "MotionFilter.h"
#include "BeatDetector.h"
#include "StartGestureDetector.h"
#include "VisualizationObserver.h"
#include "TrackingStateObserver.h"

namespace cf {
namespace MotionTracker {

class EventProviderImpl
	: public EventProvider
	, public HandObserver
	, public VisualizationObserver
	, public TrackingStateObserver
{

private:
	struct Queue : public EventQueue
	{
		typedef boost::lockfree::ringbuffer<Event, 0> InterThreadEventBuffer;

		Queue()
			: eventBuffer(256)
		{}

		bool DequeueEvent(Event & result)
		{
			return eventBuffer.dequeue(result);
		}

		InterThreadEventBuffer eventBuffer;
	};

public:
	EventProviderImpl();
	~EventProviderImpl();

public: // EventProvider implementation
	void StartProduction();
	void StopProduction();
	boost::shared_ptr<EventQueue> GetEventQueue();
	MusicalContextBuffer::Writer GetMusicalContextWriter();

public: // HandObserver implementation, called from tracker thread
	void HandFound();
	void HandLost();
	void NewHandPosition(float time, Point3D const & pos);

public: // VisualizationObserver implementation
	void InitVisualizationData(int width, int height);
	Visualizer::DataPtr GetVisualizationData();
	void NewVisualizationData();
	void NewVisualizationHandPosition(Visualizer::Position const & pos);

public: // TrackingStateObserver implementation
	void TrackingStateChanged(TrackingState const & newState);

private:
	void QueueEvent(Event const & e);
	void CleanUpQueues();
	void RunMotionFilters(timestamp_t const & timeNow, MotionState const & state);
	bool DetectBeat(timestamp_t const & timeNow, MotionState const & state);
	void DetectStartGesture(timestamp_t const & timeNow, MotionState const & state, bool beatOccurred);

private: // tracker thread state and event buffer
	GlobalsRef globalsRef_;
	class TrackerThread;

	boost::shared_ptr<LockfreeThread<TrackerThread> > trackerThread_;
	boost::shared_ptr<HandTracker> tracker_;
	std::vector<boost::shared_ptr<Queue> > queues_;
	boost::mutex queueMutex_;

	MusicalContextBuffer musicalContextBuffer_;
	MusicalContextBuffer::Reader musicalContextReader_;

private: // Filters and detectors

	MotionFilter motionFilter_;

	BeatDetector beatDetector_;
	StartGestureDetector startDetector_;

	EMA<4> velocityFir_;
	PeakHolder<20> velocityPeakHolder_;
	DynamicRange<20> velocityRange_;

	AveragingFir<10> jerkFir_;
	DipHolder<40> jerkPeakHolder_;

	StdDev<20> velocityDev_;

private: // Visualization stuff
	Visualizer::DataPtr visualizationData_;
	Visualizer::DataBufferPtr visualizationBuffer_;
};

} // namespace MotionTracker
} // namespace cf
