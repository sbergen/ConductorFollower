#pragma once

#include <cmath>

#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/units/systems/si/prefixes.hpp> // include for convenience
#include <boost/units/pow.hpp>
#include <boost/units/cmath.hpp>

#include "cf/Vector.h"

#define REGISTER_VECTOR_AS_POINT(type) \
	BOOST_GEOMETRY_REGISTER_POINT_3D_GET_SET(type, type::raw_type, boost::geometry::cs::cartesian, raw_x, raw_y, raw_z, set_raw_x, set_raw_y, set_raw_z)

REGISTER_VECTOR_AS_POINT(cf::Point3D);
REGISTER_VECTOR_AS_POINT(cf::Velocity3D);

namespace cf {

typedef Point3D::quantity coord_t;
typedef Velocity3D::quantity velocity_t;

typedef boost::geometry::model::box<Point3D> Box3D;
typedef boost::geometry::model::linestring<Point3D> Linestring3D;

template<typename T>
struct IteratorLinestring : public boost::iterator_range<T>
{ IteratorLinestring(T const & begin, T const & end) : boost::iterator_range<T>(begin, end) {} };

namespace geometry
{
	template<typename T>
	T distance_vector(T const & from, T const & to)
	{
		T result(to);
		boost::geometry::subtract_point(result, from);
		return result;
	}

	template<typename T>
	inline coord_t abs(T const & point)
	{
		return boost::units::sqrt(
			boost::units::pow<2>(point.get_x()) +
			boost::units::pow<2>(point.get_y()) +
			boost::units::pow<2>(point.get_z()));
	}

} // namespace geometry
	
} // namespace cf

BOOST_GEOMETRY_REGISTER_LINESTRING_TEMPLATED(cf::IteratorLinestring)
