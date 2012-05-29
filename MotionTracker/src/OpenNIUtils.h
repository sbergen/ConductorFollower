#pragma once

#include <string>
#include <iostream>

#include <XnCppWrapper.h>

namespace cf {
namespace MotionTracker {

#define CheckXnStatus(status, desc) OpenNIUtils::CheckStatus(status, desc, __FILE__, __LINE__)

class OpenNIUtils
{
public:
	static bool CheckStatus(XnStatus status, std::string const & taskDescription, std::string const & file, int line);

	static void SetErrorStream(std::ostream & stream) { err_stream = &stream; }

	static void ResetErrors() { errorsOccurred_ = false; }
	static bool ErrorsOccurred() { return errorsOccurred_; }

private:
	static bool errorsOccurred_;
	static std::ostream * err_stream;
};

} // namespace MotionTacker
} // namespace cf
