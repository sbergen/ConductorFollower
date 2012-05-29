#pragma once

#include "OpenNIUtils.h"

class GlobalSetup
{
public:
	GlobalSetup()
		: nullStream_(0)
	{
		cf::MotionTracker::OpenNIUtils::SetErrorStream(nullStream_);
	}

	~GlobalSetup()
	{

	}

private:
	std::ostream nullStream_;

};