#include <boost/test/unit_test.hpp>

#include "cf/Fir.h"

BOOST_AUTO_TEST_SUITE(FirTests)

using namespace cf;

BOOST_AUTO_TEST_CASE(TestAveragingFir)
{
	AveragingFir<10> fir;
	double value = 1;
	value = fir.Run(value);

	BOOST_CHECK_CLOSE(value, 0.1, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()