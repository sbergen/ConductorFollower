#include <boost/test/unit_test.hpp>

#include <boost/units/systems/si.hpp>

#include "cf/unit_helpers.h"

using namespace cf;

namespace bu = boost::units;
namespace si = bu::si;

BOOST_AUTO_TEST_SUITE(UnitHelperTests)


BOOST_AUTO_TEST_CASE(TestTwoPointIntegral)
{
	bu::quantity<si::time> from(1.0 * si::seconds);
	bu::quantity<si::time> to(2.0 * si::seconds);
	bu::quantity<si::length> result = unit_integral<si::velocity>(
		from, to,
		[](double x)
		{
			return 0.5 * x * x;
		});

	BOOST_CHECK_CLOSE(result.value(), 1.5, 0.001);
}

BOOST_AUTO_TEST_CASE(TestOnePointIntegral)
{
	bu::quantity<si::time> to(2.0 * si::seconds);
	bu::quantity<si::length> result = unit_integral<si::velocity>(
		to,
		[](double x)
		{
			return 0.5 * x * x;
		});

	BOOST_CHECK_CLOSE(result.value(), 2.0, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()