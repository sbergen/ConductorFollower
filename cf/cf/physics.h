#pragma once

#include <boost/units/systems/si/prefixes.hpp> // include for convenience

#include "cf/geometry.h"
#include "cf/time.h"

namespace cf {

typedef VectorND<3, boost::units::si::velocity> Velocity3D;
typedef VectorND<3, boost::units::si::acceleration> Acceleration3D;

typedef Velocity3D::quantity velocity_t;
typedef Acceleration3D::quantity acceleration_t;

// Velocity from movement over time
inline Velocity3D operator/ (Point3D const & movement, time_quantity const & time)
{
	Velocity3D ret;
	movement.transform(ret,
		[&time](Point3D::quantity const & coordinate) -> velocity_t { return coordinate / time; });
	return ret;
}

} // namespace cf

REGISTER_VECTOR_AS_POINT(cf::Velocity3D);
REGISTER_VECTOR_AS_POINT(cf::Acceleration3D);
