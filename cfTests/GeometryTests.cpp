#include <boost/test/unit_test.hpp>

#include "cf/geometry.h"

using namespace cf;

BOOST_AUTO_TEST_SUITE(GeometryTests)

BOOST_AUTO_TEST_CASE(TestDistance)
{
	Point3D a, b;
	a.set_raw<0>(1);
	a.set_raw<1>(1);
	a.set_raw<2>(1);

	b.set_raw<0>(3);
	b.set_raw<1>(4);
	b.set_raw<2>(5);

	auto distanceVec = geometry::distance_vector(a, b);
	BOOST_CHECK_CLOSE(distanceVec.get_raw<0>(), 2.0, 0.01);
	BOOST_CHECK_CLOSE(distanceVec.get_raw<1>(), 3.0, 0.01);
	BOOST_CHECK_CLOSE(distanceVec.get_raw<2>(), 4.0, 0.01);
}

BOOST_AUTO_TEST_CASE(TestAbsolute)
{
	Point3D a;
	a.set_raw<0>(1);
	a.set_raw<1>(2);
	a.set_raw<2>(3);

	auto abs = geometry::abs(a);
	double calculated = std::sqrt(std::pow(1.0, 2) + std::pow(2.0, 2) + std::pow(3.0, 2));
	BOOST_CHECK_CLOSE(abs.value(), calculated, 0.01);
}

BOOST_AUTO_TEST_SUITE_END()