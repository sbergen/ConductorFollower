#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace cf {
namespace MotionTracker {

class Event;

class EventProvider : public boost::noncopyable
{
public:
	// Hide implementation
	static boost::shared_ptr<EventProvider> Create();

public:
	virtual ~EventProvider() {}

	// Start producing data in separate thread
	// returns false if production is already started
	virtual bool StartProduction() = 0;

	// Will eventually stop producing data
	// returns false if production is not running
	virtual bool StopProduction() = 0;

	// Lock free getter for events
	// Returns false if there's no data available
	virtual bool DequeueEvent(Event & result) = 0;

};

} // namespace MotionTracker
} // namespace cf
