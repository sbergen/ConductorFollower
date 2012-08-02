#pragma once

#include <boost/function.hpp>
#include <boost/utility.hpp>

#include "MotionTracker/Event.h"

namespace cf {
namespace MotionTracker {

class EventProvider;

class EventThrottler : public boost::noncopyable
{
public:
	EventThrottler(EventProvider & provider)
		: provider_(provider) {}

	template<typename Consumer>
	void ConsumeEventsUntil(Consumer const & consumer, timestamp_t const & time)
	{
		if (queuedEvent_.isQueued) {
			consumer(queuedEvent_.e);
			queuedEvent_.isQueued = false;
		}

		while (provider_.DequeueEvent(queuedEvent_.e)) {
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

	EventProvider & provider_;
	QueuedEvent queuedEvent_;
};

} // namespace MotionTracker
} // namespace cf
