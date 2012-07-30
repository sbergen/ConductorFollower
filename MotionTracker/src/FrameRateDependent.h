#pragma once

namespace cf {
namespace MotionTracker {

struct FrameRateDependent
{
	enum {
		frame_rate = 30,
		filter_size = 7,
		filter_order = 3
	};
};


} // namespace MotionTracker
} // namespace cf
