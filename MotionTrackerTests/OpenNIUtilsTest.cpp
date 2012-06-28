#include <boost/test/unit_test.hpp>

#include "OpenNIUtils.h"

BOOST_AUTO_TEST_SUITE(OpenNIUtilsTests)

using namespace cf::MotionTracker;

BOOST_AUTO_TEST_CASE(Stati)
{
	BOOST_CHECK(CheckXnStatus(XN_STATUS_OK, "Should not fail"));
	BOOST_CHECK(!CheckXnStatus(XN_STATUS_ERROR, "Failing on purpose"));
}

BOOST_AUTO_TEST_CASE(ErrorCatching)
{
	OpenNIUtils::ResetErrors();
	BOOST_CHECK(!OpenNIUtils::ErrorsOccurred());

	CheckXnStatus(XN_STATUS_ERROR, "Setting error state on purpose");
	BOOST_CHECK(OpenNIUtils::ErrorsOccurred());

	CheckXnStatus(XN_STATUS_OK, "ok status");
	BOOST_CHECK(OpenNIUtils::ErrorsOccurred());

	OpenNIUtils::ResetErrors();
	BOOST_CHECK(!OpenNIUtils::ErrorsOccurred());
}

BOOST_AUTO_TEST_SUITE_END()