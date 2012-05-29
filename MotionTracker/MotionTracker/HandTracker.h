#pragma once

#include "Gestures.h"

namespace cf {
namespace MotionTracker {

class HandObserver;

class HandTracker
{
public:
	// Factory function to hide dependencies to implementations
	static HandTracker * Create();

public:
	virtual ~HandTracker() {}

	// Initialization
	virtual bool Init() = 0;

	// Basic tracking
	virtual bool StartTrackingHand(Gesture gesture, HandObserver & observer) = 0;
	virtual bool StopTrackingHand(HandObserver & observer) = 0;

	// "event loop"
	virtual bool WaitForData() = 0;
};

} // namespace cf
} // namespace MotionTracker
