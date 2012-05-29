#include "OpenNIUtils.h"

#include <boost/format.hpp>

namespace cf {
namespace MotionTracker {

bool OpenNIUtils::errorsOccurred_ = false;
std::ostream * OpenNIUtils::err_stream = &std::cerr;

bool
OpenNIUtils::CheckStatus(XnStatus status, std::string const & taskDescription, std::string const & file, int line)
{
	if (status == XN_STATUS_OK) { return true; }

	*err_stream
		<< boost::format("'%1%' failed with status '%2%' (%3%:%4%)")
		% taskDescription
		% xnGetStatusString(status)
		% file
		% line
		<< std::endl;

	errorsOccurred_ = true;
	return false;
}

} // namespace cf
} // namespace MotionTracker
