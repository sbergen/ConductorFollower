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
	EventThrottler(EventProvider & provider);

	typedef boost::function<void (Event const &)> Consumer;
	void ConsumeEventsUntil(Consumer const & consumer, timestamp_t const & time);

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
