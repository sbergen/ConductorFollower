#pragma once

#include "cf/geometry.h"
#include "cf/time.h"

namespace cf {

// Velocity from movement over time
Velocity3D operator/ (Point3D const & movement, time_quantity const & time)
{
	return movement.transform<Velocity3D>(
		[&time](Point3D::quantity const & coordinate) { return coordinate / time; });
}

} // namespace cf
