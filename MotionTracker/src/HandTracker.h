#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "Gestures.h"

namespace cf {
namespace MotionTracker {

class HandObserver;
class VisualizationObserver;

class HandTracker : public boost::noncopyable
{
public:
	// Factory function to hide dependencies to implementations
	static boost::shared_ptr<HandTracker> Create();

public:
	virtual ~HandTracker() {}

	// Initialization
	virtual bool Init() = 0;

	// Basic tracking
	virtual bool StartTrackingHand(Gesture gesture, HandObserver & observer) = 0;
	virtual bool StopTrackingHand(HandObserver & observer) = 0;

	// Visualization data
	virtual void AddVisualizationObserver(VisualizationObserver & observer) = 0;

	// "event loop"
	virtual bool WaitForData() = 0;
};

} // namespace MotionTracker
} // namespace cf
