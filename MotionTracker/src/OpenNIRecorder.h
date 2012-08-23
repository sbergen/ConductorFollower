#pragma once

#include <XnCppWrapper.h>

#include "OpenNIUtils.h"

namespace cf {
namespace MotionTracker {

class OpenNIRecorder
{
public:
	enum Mode 
	{
		Record,
		Playback,
		Disabled
	};

public:
	OpenNIRecorder(Mode mode, XnChar const * filename);
	~OpenNIRecorder();

	void PreNodeInit(xn::Context & context);
	void PostNodeInit(xn::Context & context);

private:
	Mode mode_;
	XnChar const * filename_;
	OpenNIUtils utils_;
	xn::Recorder recorder_;
	xn::Player player_;
};

} // namespace MotionTacker
} // namespace cf
