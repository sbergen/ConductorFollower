#pragma once

#include "cf/geometry.h"
#include "cf/time.h"

namespace cf {

// Velocity from movement over time
Velocity3D operator/ (Point3D const & movement, time_quantity const & time)
{
	Velocity3D ret;
	movement.transform(ret,
		[&time](Point3D::quantity const & coordinate) -> velocity_t { return coordinate / time; });
	return ret;
}

} // namespace cf
