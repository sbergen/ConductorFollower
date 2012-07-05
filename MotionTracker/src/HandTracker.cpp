#include "HandTracker.h"

#include <boost/make_shared.hpp>

#include "OpenNIHandTracker.h"

namespace cf {
namespace MotionTracker {

boost::shared_ptr<HandTracker>
HandTracker::Create()
{
	// No other implementations ATM
	return boost::make_shared<OpenNIHandTracker>();
}

} // namespace MotionTracker
} // namespace cf
