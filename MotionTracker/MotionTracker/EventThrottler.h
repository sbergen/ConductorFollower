#pragma once

#include <boost/function.hpp>
#include <boost/utility.hpp>

#include "MotionTracker/Event.h"
#include "MotionTracker/EventQueue.h"

namespace cf {
namespace MotionTracker {

class EventQueue;

class EventThrottler : public boost::noncopyable
{
public:
	typedef boost::shared_ptr<EventQueue> QueuePtr;

	EventThrottler(QueuePtr queue)
		: queue_(queue) {}

	template<typename Consumer>
	void ConsumeEventsUntil(Consumer const & consumer, timestamp_t const & time)
	{
		if (queuedEvent_.isQueued) {
			consumer(queuedEvent_.e);
			queuedEvent_.isQueued = false;
		}

		while (queue_->DequeueEvent(queuedEvent_.e)) {
			if (queuedEvent_.e.timestamp() >= time) {
				queuedEvent_.isQueued = true;
				break;
			}
			consumer(queuedEvent_.e);
		}
	}


private:
	struct QueuedEvent
	{
		QueuedEvent() : isQueued(false) {}
		bool isQueued;
		Event e;
	};

	QueuePtr queue_;
	QueuedEvent queuedEvent_;
};

} // namespace MotionTracker
} // namespace cf
