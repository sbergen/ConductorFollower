#include "cf/EventBuffer.h"

#include <boost/range/numeric.hpp>

using namespace cf;

BOOST_AUTO_TEST_SUITE(EventBufferTests)

class NoOperatorType
{
};

BOOST_AUTO_TEST_CASE(TestNoOperatorType)
{
	// Check that this compiles even without arithmetic or comparison operators
	EventBuffer<NoOperatorType, int> buffer(3);
	BOOST_CHECK(true); // suppress the warnings, as this is a compile time check
}

BOOST_AUTO_TEST_CASE(DataSince)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSince(0.0).dataRange(), 0), 6);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSince(0.1).dataRange(), 0), 6);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSince(0.2).dataRange(), 0), 5);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSince(0.3).dataRange(), 0), 3);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSince(0.4).dataRange(), 0), 0);
}

BOOST_AUTO_TEST_CASE(EventsBetween)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);

	// past the end
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(0, 0.3).dataRange(), 0), 3);

	// single element
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(0.1, 0.1).dataRange(), 0), 1);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(0.2, 0.2).dataRange(), 0), 2);

	// "regular access"
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(0.1, 0.25).dataRange(), 0), 3);

	// no elements (before and after)
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(0.0, 0.01).dataRange(), 0), 0);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(0.4, 0.5).dataRange(), 0), 0);
}

BOOST_AUTO_TEST_CASE(LastTimestamp)
{
	EventBuffer<int, double> buffer(3);
	BOOST_CHECK_EQUAL(buffer.LastTimestamp(), int());

	buffer.RegisterEvent(0.1, 1);
	BOOST_CHECK_EQUAL(buffer.LastTimestamp(), 0.1);

	buffer.RegisterEvent(0.2, 2);
	BOOST_CHECK_EQUAL(buffer.LastTimestamp(), 0.2);

	buffer.RegisterEvent(0.3, 3);
	BOOST_CHECK_EQUAL(buffer.LastTimestamp(), 0.3);
}

BOOST_AUTO_TEST_CASE(TimeStampsSince)
{
	EventBuffer<int, int> buffer(3);
	buffer.RegisterEvent(1, 1);
	buffer.RegisterEvent(2, 2);
	buffer.RegisterEvent(3, 3);

	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSince(0).timestampRange(), 0), 6);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSince(1).timestampRange(), 0), 6);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSince(2).timestampRange(), 0), 5);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSince(3).timestampRange(), 0), 3);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSince(4).timestampRange(), 0), 0);
}

BOOST_AUTO_TEST_CASE(TimeStampsSinceInclusive)
{
	EventBuffer<int, int> buffer(3);
	buffer.RegisterEvent(1, 1);
	buffer.RegisterEvent(2, 2);
	buffer.RegisterEvent(4, 3);

	// This is a special case. The timestamps do not include 0, so the range returned is empty
	BOOST_CHECK(buffer.EventsSinceInclusive(0).Empty());

	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSinceInclusive(1).timestampRange(), 0), 7);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSinceInclusive(2).timestampRange(), 0), 6);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSinceInclusive(3).timestampRange(), 0), 6);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSinceInclusive(4).timestampRange(), 0), 4);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsSinceInclusive(5).timestampRange(), 0), 0);
}

BOOST_AUTO_TEST_CASE(TimeStampsBetween)
{
	EventBuffer<int, int> buffer(3);
	buffer.RegisterEvent(1, 1);
	buffer.RegisterEvent(2, 2);

	// past the end
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(0, 4).timestampRange(), 0), 3);

	// single element
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(1, 1).timestampRange(), 0), 1);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(2, 2).timestampRange(), 0), 2);

	// "regular access"
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(1, 2).timestampRange(), 0), 3);

	// no elements (before and after)
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(0, 0).timestampRange(), 0), 0);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.EventsBetween(4, 5).timestampRange(), 0), 0);
}


BOOST_AUTO_TEST_CASE(ContainsDataAfter)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	BOOST_CHECK(!buffer.ContainsDataAfter(0.0));
	BOOST_CHECK(buffer.ContainsDataAfter(0.1));
	BOOST_CHECK(buffer.ContainsDataAfter(0.3));
	BOOST_CHECK(!buffer.ContainsDataAfter(0.4));

	// This one should overwrite
	buffer.RegisterEvent(0.4, 4);
	BOOST_CHECK(!buffer.ContainsDataAfter(0.1));
	BOOST_CHECK(buffer.ContainsDataAfter(0.2));
	BOOST_CHECK(buffer.ContainsDataAfter(0.4));
	BOOST_CHECK(!buffer.ContainsDataAfter(0.5));
}

BOOST_AUTO_TEST_SUITE_END()