#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/lockfree/ringbuffer.hpp>

#include "cf/LockfreeThread.h"
#include "cf/time.h"
#include "cf/geometry.h"
#include "cf/globals.h"

#include "MotionTracker/EventProvider.h"

#include "common_types.h"
#include "HandObserver.h"
#include "HandTracker.h"
#include "MotionFilter.h"
#include "BeatDetector.h"

namespace cf {
namespace MotionTracker {

class EventProviderImpl : public EventProvider, public HandObserver
{
public:
	typedef boost::lockfree::ringbuffer<Event, 0> InterThreadEventBuffer;

public:
	EventProviderImpl();
	~EventProviderImpl();

public: // EventProvider implementation
	void StartProduction();
	void StopProduction();
	bool DequeueEvent(Event & result);

public: // HandObserver implementation, called from tracker thread
	void HandFound();
	void HandLost();
	void NewHandPosition(float time, Point3D const & pos);

private: // tracker thread state and event buffer
	GlobalsRef globalsRef_;
	class TrackerThread;

	boost::shared_ptr<LockfreeThread<TrackerThread> > trackerThread_;
	InterThreadEventBuffer eventBuffer_;
	MotionFilter motionFilter_;
	BeatDetector beatDetector_;
};

} // namespace MotionTracker
} // namespace cf
