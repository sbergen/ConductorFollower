#include "OpenNIUtils.h"

#include <boost/format.hpp>

namespace cf {
namespace MotionTracker {

bool
OpenNIUtils::CheckStatus(XnStatus status, char const * taskDescription, char const * file, int line)
{
	if (status == XN_STATUS_OK) { return true; }

	// The error here will cause an allocation, but we don't really care at this stage...
	errorStream_
		<< boost::format("'%1%' failed with status '%2%' (%3%:%4%)")
		% taskDescription
		% xnGetStatusString(status)
		% file
		% line
		<< std::endl;

	errorsOccurred_ = true;
	return false;
}

} // namespace MotionTracker
} // namespace cf
