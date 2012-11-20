#pragma once

#include <algorithm>
#include <numeric>
#include <cmath>

#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/core/coordinate_system.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/units/systems/si.hpp>

#include "cf/VectorND.h"

namespace cf {

typedef VectorND<3, boost::units::si::length> Point3D;
typedef VectorND<2, boost::units::si::length> Point2D;

typedef Point3D::quantity distance_t;
typedef distance_t coord_t;

typedef boost::geometry::model::box<Point3D> Box3D;

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
	inline typename T::quantity abs(T const & point)
	{
		// Skip dimensional analysis here...
		typename T::data_type temp(T::dimension);
		auto const & data = point.data();

		// Binding to pow doesn't work because of overloads...
		std::transform(std::begin(data), std::end(data), std::begin(temp),
			[](typename T::raw_type const & v) { return std::pow(v, 2); });
		auto val = std::sqrt(std::accumulate(std::begin(temp), std::end(temp), T::raw_type()));
		return T::quantity::from_value(val);
	}

} // namespace geometry

} // namespace cf

#define REGISTER_VECTOR_AS_POINT(type) \
	BOOST_GEOMETRY_REGISTER_POINT_3D_GET_SET(type, type::raw_type, boost::geometry::cs::cartesian, \
	get_raw<cf::coord::X>, get_raw<cf::coord::Y>, get_raw<cf::coord::Z>, \
	set_raw<cf::coord::X>, set_raw<cf::coord::Y>, set_raw<cf::coord::Z>)

#define REGISTER_VECTOR_AS_POINT_2D(type) \
	BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(type, type::raw_type, boost::geometry::cs::cartesian, \
	get_raw<cf::coord::X>, get_raw<cf::coord::Y>, \
	set_raw<cf::coord::X>, set_raw<cf::coord::Y>)

REGISTER_VECTOR_AS_POINT(cf::Point3D);
REGISTER_VECTOR_AS_POINT_2D(cf::Point2D);

BOOST_GEOMETRY_REGISTER_LINESTRING_TEMPLATED(cf::IteratorLinestring)
