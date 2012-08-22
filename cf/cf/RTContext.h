#pragma once

#include <boost/utility.hpp>

#ifdef NDEBUG

namespace cf {
	class RTContext {};
	class NonRTSection {};
}

#else

#include "cf/debug_new.h"

namespace cf {

class RTContext : public boost::noncopyable
{
public:
	RTContext()
	{
		disallow_new();
	}

	~RTContext()
	{
		 allow_new();
	}
};

class NonRTSection : public boost::noncopyable
{
public:
	NonRTSection()
		: wasRT_(new_disallowed())
	{
		if (wasRT_) { allow_new(); }
	}

	~NonRTSection()
	{
		if (wasRT_) { disallow_new(); }
	}

private:
	bool const wasRT_;
};

} // namespace cf

#endif
