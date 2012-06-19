#pragma once

#include <boost/scoped_ptr.hpp>
#include <boost/lockfree/ringbuffer.hpp>

#include "cf/cf.h"
#include "cf/EventBuffer.h"
#include "cf/LockfreeThread.h"

#include "MotionTracker/EventProvider.h"

#include "HandObserver.h"
#include "HandTracker.h"

namespace cf {
namespace MotionTracker {

class EventProviderImpl : public EventProvider, public HandObserver
{
private:
	typedef EventBuffer<Point3D, timestamp_t> PositionBuffer;
	typedef boost::lockfree::ringbuffer<Event, 0> InterThreadEventBuffer;

public:
	EventProviderImpl();
	~EventProviderImpl();

public: // EventProvider implementation
	bool StartProduction();
	bool StopProduction();
	bool DequeueEvent(Event & result);

public: // HandObserver implementation, called from tracker thread
	void HandFound();
	void HandLost();
	void NewHandPosition(float time, Point3D const & pos);

private: // tracker thread state and event buffer
	bool Init();
	bool EventLoop();
	void Cleanup();

	boost::scoped_ptr<LockfreeThread> trackerThread_;
	boost::scoped_ptr<HandTracker> tracker_;
	InterThreadEventBuffer eventBuffer_;
};

} // namespace MotionTracker
} // namespace cf
