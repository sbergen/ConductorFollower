#pragma once

#include "MotionTracker/Event.h"
#include <boost/lockfree/ringbuffer.hpp>

namespace cf {
namespace MotionTracker {

typedef boost::lockfree::ringbuffer<Event, 0> InterThreadEventBuffer;

} // namespace MotionTracker
} // namespace cf
