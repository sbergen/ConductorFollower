#include "cf/EventBuffer.h"

#include <boost/range/numeric.hpp>

using namespace cf;
using namespace cf::FeatureExtractor;

BOOST_AUTO_TEST_SUITE(EventBufferTests)

class NoOperatorType
{
};

BOOST_AUTO_TEST_CASE(TestNoOperatorType)
{
	// Check that this compiles even without arithmetic or comparison operators
	EventBuffer<NoOperatorType> buffer(3);
	BOOST_CHECK(true); // suppress the warnings, as this is a compile time check
}

BOOST_AUTO_TEST_CASE(DataAccess)
{
	EventBuffer<int> buffer(5);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);

	BOOST_CHECK_THROW(buffer.DataAt(0.0), std::out_of_range);
	BOOST_CHECK_EQUAL(buffer.DataAt(0.1), 1);
	BOOST_CHECK_THROW(buffer.DataAt(0.15), std::out_of_range);
	BOOST_CHECK_EQUAL(buffer.DataAt(0.2), 2);
	BOOST_CHECK_THROW(buffer.DataAt(0.3), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(LastTimestamp)
{
	EventBuffer<int> buffer(3);
	BOOST_CHECK_EQUAL(buffer.LastTimestamp(), int());

	buffer.RegisterEvent(0.1, 1);
	BOOST_CHECK_EQUAL(buffer.LastTimestamp(), 0.1);

	buffer.RegisterEvent(0.2, 2);
	BOOST_CHECK_EQUAL(buffer.LastTimestamp(), 0.2);

	buffer.RegisterEvent(0.3, 3);
	BOOST_CHECK_EQUAL(buffer.LastTimestamp(), 0.3);
}

BOOST_AUTO_TEST_CASE(DataSince)
{
	EventBuffer<int> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	BOOST_CHECK_EQUAL(boost::accumulate(buffer.DataSince(0.0), 0), 6);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.DataSince(0.1), 0), 6);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.DataSince(0.2), 0), 5);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.DataSince(0.3), 0), 3);
	BOOST_CHECK_EQUAL(boost::accumulate(buffer.DataSince(0.4), 0), 0);
}

BOOST_AUTO_TEST_CASE(ContainsDataAfter)
{
	EventBuffer<int> buffer(3);
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