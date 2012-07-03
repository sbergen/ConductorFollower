#include "MotionTracker/EventThrottler.h"

#include "MotionTracker/EventProvider.h"

namespace cf {
namespace MotionTracker {

EventThrottler::EventThrottler(EventProvider & provider)
	: provider_(provider)
{
}

void
EventThrottler::ConsumeEventsUntil(Consumer const & consumer, timestamp_t const & time)
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

} // namespace MotionTracker
} // namespace cf
