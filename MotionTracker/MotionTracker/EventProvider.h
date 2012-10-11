#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "MotionTracker/MusicalContext.h"

namespace cf {
namespace MotionTracker {

class EventQueue;

class EventProvider : public boost::noncopyable
{
public:
	// Hide implementation
	static boost::shared_ptr<EventProvider> Create();

public:
	virtual ~EventProvider() {}

	// Do non-realtime operations that needs to happen post construction
	virtual void Init() {}

	// Will eventually start producing data in separate thread
	// If previously stopped, a restart is not quaranteed.
	virtual void StartProduction() = 0;

	// Will eventually stop producing data
	// If started again soon, a stop is not guaranteed
	virtual void StopProduction() = 0;

	// Create a new event queue for yourself,
	// all events created go to all created queues
	virtual boost::shared_ptr<EventQueue> GetEventQueue() = 0;

	// Get the musical context writer
	virtual MusicalContextBuffer::Writer GetMusicalContextWriter() = 0;
};

} // namespace MotionTracker
} // namespace cf
