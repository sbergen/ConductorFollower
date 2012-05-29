#include "MotionTracker/HandTracker.h"

#include "OpenNIHandTracker.h"

namespace cf {
namespace MotionTracker {

HandTracker* HandTracker::Create()
{
	// No other implementations ATM
	return new OpenNIHandTracker();
}

} // namespace cf
} // namespace MotionTracker
