#pragma once

#include <boost/shared_ptr.hpp>

#include "cf/physics.h"

#include "Visualizer/Data.h"

namespace cf {
namespace MotionTracker {

struct MotionState
{
	Point3D position;
	Velocity3D velocity;
	Acceleration3D acceleration;
	Jerk3D jerk;

	boost::shared_ptr<Visualizer::Data> visualizationData;
};

} // namespace MotionTracker
} // namespace cf
