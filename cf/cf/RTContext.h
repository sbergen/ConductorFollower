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
		detail::disallow_new();
	}

	~RTContext()
	{
		 detail::allow_new();
	}
};

class NonRTSection : public boost::noncopyable
{
public:
	NonRTSection()
		: wasRT_(!detail::new_allowed())
	{
		if (wasRT_) { detail::allow_new(); }
	}

	~NonRTSection()
	{
		if (wasRT_) { detail::disallow_new(); }
	}

private:
	bool const wasRT_;
};

} // namespace cf

#endif
