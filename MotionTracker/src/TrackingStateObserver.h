#pragma once

#include <boost/utility.hpp>

#include "MotionTracker/TrackingState.h"

namespace cf {
namespace MotionTracker {

class TrackingStateObserver : public boost::noncopyable
{
public:
	virtual ~TrackingStateObserver() {}

	virtual void TrackingStateChanged(TrackingState const & newState) = 0;
};

} // namespace MotionTracker
} // namespace cf
