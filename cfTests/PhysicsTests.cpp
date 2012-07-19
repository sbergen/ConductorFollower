#include <boost/test/unit_test.hpp>

#include "cf/physics.h"

using namespace cf;

BOOST_AUTO_TEST_SUITE(PhysicsTests)

namespace si = boost::units::si;

BOOST_AUTO_TEST_CASE(TestVelocityDivision)
{
	Point3D movement(10.0 * si::meters, 4.0  * si::meters, 2.0  * si::meters);
	time_quantity time(2.0 * si::seconds);
	Velocity3D velocity = movement / time;

	BOOST_CHECK_CLOSE(velocity.get_raw<0>(), 5.0, 0.01);
	BOOST_CHECK_CLOSE(velocity.get_raw<1>(), 2.0, 0.01);
	BOOST_CHECK_CLOSE(velocity.get_raw<2>(), 1.0, 0.01);
}

BOOST_AUTO_TEST_SUITE_END()