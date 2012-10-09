#pragma once

#include "cf/physics.h"

#include "Visualizer/Data.h"

namespace cf {
namespace MotionTracker {

struct MotionState
{
	Point3D position; // unfiltered
	Velocity3D fastVelocity; // fast filter

	// slow filter
	Velocity3D velocity;
	Acceleration3D acceleration;
	Jerk3D jerk;
};

} // namespace MotionTracker
} // namespace cf
