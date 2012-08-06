#pragma once

#ifdef NDEBUG

namespace cf {
	class RTContext {};
}

#else

#include "cf/debug_new.h"

namespace cf {

class RTContext
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

} // namespace cf

#endif
