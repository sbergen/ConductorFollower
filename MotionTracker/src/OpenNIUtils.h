#pragma once

#include <string>
#include <iostream>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/utility.hpp>

#include <XnCppWrapper.h>

namespace cf {
namespace MotionTracker {

#define CheckXnStatus(status, desc) OpenNIUtils::CheckStatus(status, desc, __FILE__, __LINE__)

class OpenNIUtils : public boost::noncopyable
{
public:
	static bool CheckStatus(XnStatus status, std::string const & taskDescription, std::string const & file, int line);

	static void SetErrorStream(std::ostream & stream) { ErrorStream().store(&stream); }

	static void ResetErrors() { errorsOccurred_ = false; }
	static bool ErrorsOccurred() { return errorsOccurred_; }

private:
	static boost::atomic<std::ostream*> & ErrorStream()
	{
		static boost::atomic<std::ostream*> stream(&std::cerr);
		return stream;
	}

	static bool errorsOccurred_;
};

} // namespace MotionTacker
} // namespace cf
