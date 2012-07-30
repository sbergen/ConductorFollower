#include <boost/test/unit_test.hpp>

#include "cf/PeakHolder.h"

BOOST_AUTO_TEST_SUITE(PeakHolderTests)

using namespace cf;

BOOST_AUTO_TEST_CASE(TestPeakHolder)
{
	PeakHolder<3> ph;
	
	BOOST_CHECK_CLOSE(ph.Run(0.0), 0.0, 0.001);
	BOOST_CHECK_CLOSE(ph.Run(0.1), 0.1, 0.001);
	BOOST_CHECK_CLOSE(ph.Run(0.2), 0.2, 0.001);
	BOOST_CHECK_CLOSE(ph.Run(0.3), 0.3, 0.001);
	BOOST_CHECK_CLOSE(ph.Run(0.2), 0.3, 0.001);
	BOOST_CHECK_CLOSE(ph.Run(0.1), 0.3, 0.001);
	BOOST_CHECK_CLOSE(ph.Run(0.0), 0.2, 0.001);
	BOOST_CHECK_CLOSE(ph.Run(0.0), 0.1, 0.001);
	BOOST_CHECK_CLOSE(ph.Run(0.0), 0.0, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()