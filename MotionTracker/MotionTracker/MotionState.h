#pragma once

#include "cf/physics.h"

namespace cf {
namespace MotionTracker {

struct MotionState
{
	Point3D position;
	Velocity3D velocity;
	Acceleration3D acceleration;
	Jerk3D jerk;
};

} // namespace MotionTracker
} // namespace cf
