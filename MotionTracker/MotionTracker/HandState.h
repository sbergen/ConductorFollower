#pragma once

namespace cf {
namespace MotionTracker {

struct HandState
{
	enum Which
	{
		Left,
		Right
	};

	enum State
	{
		Found,
		Lost
	};

	Which which;
	State state;
};

} // namespace MotionTracker
} // namespace cf
