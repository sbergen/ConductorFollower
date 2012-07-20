#include <boost/test/unit_test.hpp>

#include "cf/math.h"

BOOST_AUTO_TEST_SUITE(MathTests)

using namespace cf::math;

BOOST_AUTO_TEST_CASE(TestVectorShiftLeft)
{
	Vector x(3);
	x(0) = 0.0;
	x(1) = 1.0;
	x(2) = 2.0;

	shift_vector<-1>(x);

	BOOST_CHECK_EQUAL(x(0), 1.0);
	BOOST_CHECK_EQUAL(x(1), 2.0);
	BOOST_CHECK_EQUAL(x(2), 2.0);
}

BOOST_AUTO_TEST_CASE(TestVectorShiftRight)
{
	Vector x(3);
	x(0) = 0.0;
	x(1) = 1.0;
	x(2) = 2.0;

	shift_vector<1>(x);

	BOOST_CHECK_EQUAL(x(0), 0.0);
	BOOST_CHECK_EQUAL(x(1), 0.0);
	BOOST_CHECK_EQUAL(x(2), 1.0);
}

BOOST_AUTO_TEST_SUITE_END()