#include "OpenNIUtils.h"

#include <boost/format.hpp>

#include "cf/globals.h"
#include "cf/RTContext.h"

namespace cf {
namespace MotionTracker {

void
OpenNIUtils::CheckStatus(XnStatus status, char const * taskDescription, char const * file, int line)
{
	if (!CheckStatusNothrow(status, taskDescription, file, line)) {
		NonRTSection nonRT;

		throw std::runtime_error(
			(boost::format("'%1%' failed with status '%2%' (%3%:%4%)")
			% taskDescription
			% xnGetStatusString(status)
			% file
			% line).str());
	}
}

bool
OpenNIUtils::CheckStatusNothrow(XnStatus status, char const * taskDescription, char const * file, int line)
{
	if (status == XN_STATUS_OK) { return true; }

	// If we have an error, we don't care about RT anymore
	NonRTSection nonRT;

	// The error here will cause an allocation, but we don't really care at this stage...
	auto errorStr =
		(boost::format("'%1%' failed with status '%2%' (%3%:%4%)")
		% taskDescription
		% xnGetStatusString(status)
		% file
		% line).str();

	GlobalsRef globals;
	globals.ErrorBuffer()->enqueue(errorStr);

	errorsOccurred_ = true;
	return false;
}

} // namespace MotionTracker
} // namespace cf
