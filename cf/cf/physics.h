#pragma once

#include <boost/units/systems/si/prefixes.hpp> // include for convenience

#include "cf/geometry.h"
#include "cf/time.h"

namespace boost { namespace units {

typedef derived_dimension<length_base_dimension, 1,
                          time_base_dimension, -3>::type jerk_dimension;

namespace si {
	typedef unit<jerk_dimension, si::system> jerk;
}

} } // namespace boost units

namespace cf {

typedef VectorND<3, boost::units::si::velocity> Velocity3D;
typedef VectorND<3, boost::units::si::acceleration> Acceleration3D;
typedef VectorND<3, boost::units::si::jerk> Jerk3D;

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
