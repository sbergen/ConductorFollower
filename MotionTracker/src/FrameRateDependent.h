#pragma once

namespace cf {
namespace MotionTracker {

struct FrameRateDependent
{
	enum {
		frame_rate = 30,
		
		filter_size = 7,
		filter_order = 4,
		
		fast_filter_size = 3,
		fast_filter_order = 2
	};
};


} // namespace MotionTracker
} // namespace cf
