#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace cf {
namespace MotionTracker {

class Event;

class EventQueue : public boost::noncopyable
{
public:
	virtual ~EventQueue() {}

	// TODO add some sort of filtering?

	// Lock free getter for events
	// Returns false if there's no data available
	virtual bool DequeueEvent(Event & result) = 0;
};

} // namespace MotionTracker
} // namespace cf
