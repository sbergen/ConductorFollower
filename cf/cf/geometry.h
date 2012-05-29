#pragma once

#include <boost/geometry/core/coordinate_system.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/range/iterator_range.hpp>

namespace cf {

typedef double coord_t;

typedef boost::geometry::model::point<coord_t, 3, boost::geometry::cs::cartesian> Point3D;
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

} // namespace geometry
	
} // namespace cf

BOOST_GEOMETRY_REGISTER_LINESTRING_TEMPLATED(cf::IteratorLinestring)
