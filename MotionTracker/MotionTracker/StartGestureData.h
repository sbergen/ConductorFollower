#pragma once

#include "cf/time.h"

namespace cf {
namespace MotionTracker {

struct StartGestureData
{
	StartGestureData(timestamp_t const & preparatoryBeatTime, duration_t const & gestureDuration)
		: preparatoryBeatTime(preparatoryBeatTime), gestureDuration(gestureDuration) {}

	timestamp_t preparatoryBeatTime;
	duration_t  gestureDuration;
};

} // namespace MotionTracker
} // namespace cf
