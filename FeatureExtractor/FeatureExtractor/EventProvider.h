#pragma once

namespace cf {
namespace FeatureExtractor {

class Event;

class EventProvider
{
public:
	// Hide implementation
	static EventProvider * Create();

public:
	virtual ~EventProvider() {}

	// Start producing data in separate thread
	// returns false if production is already started
	virtual bool StartProduction() = 0;

	// Will eventually stop producing data, terminates the thread started earlier
	// returns false if production is not running
	virtual bool StopProduction() = 0;

	// Lock free getter for events
	// Returns false if there's no data available
	virtual bool DequeueEvent(Event & result) = 0;

};

} // namespace cf
} // namespace FeatureExtractor
