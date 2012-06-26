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

BOOST_AUTO_TEST_CASE(AllEvents)
{
	EventBuffer<int, int> buffer(3);
	buffer.RegisterEvent(1, 1);
	buffer.RegisterEvent(2, 2);
	buffer.RegisterEvent(3, 3);

	auto events = buffer.AllEvents();

	BOOST_CHECK_EQUAL(events.Size(), 3);

	BOOST_CHECK_EQUAL(events[0].timestamp, 1);
	BOOST_CHECK_EQUAL(events.LastTimestamp(), 3);

	BOOST_CHECK_EQUAL(events[0].data, 1);
	BOOST_CHECK_EQUAL(events.dataRange().back(), 3);
}

BOOST_AUTO_TEST_CASE(EventsSince)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	auto events = buffer.EventsSince(0.0);
	BOOST_CHECK_EQUAL(events.Size(), 3);

	events = buffer.EventsSince(0.1);
	BOOST_CHECK_EQUAL(events.Size(), 3);

	events = buffer.EventsSince(0.15);
	BOOST_CHECK_EQUAL(events.Size(), 2);

	events = buffer.EventsSince(0.2);
	BOOST_CHECK_EQUAL(events.Size(), 2);

	events = buffer.EventsSince(0.25);
	BOOST_CHECK_EQUAL(events.Size(), 1);

	events = buffer.EventsSince(0.3);
	BOOST_CHECK_EQUAL(events.Size(), 1);

	events = buffer.EventsSince(0.4);
	BOOST_CHECK_EQUAL(events.Size(), 0);
}

BOOST_AUTO_TEST_CASE(EventsSinceInclusiveNotIncluded)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	auto events = buffer.EventsSinceInclusive(0.0);
	BOOST_CHECK_EQUAL(events.Size(), 0);

	events = buffer.EventsSinceInclusive(0.05);
	BOOST_CHECK_EQUAL(events.Size(), 0);
}


BOOST_AUTO_TEST_CASE(EventsSinceInclusiveNormal)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	auto events = buffer.EventsSinceInclusive(0.1);
	BOOST_CHECK_EQUAL(events.Size(), 3);

	events = buffer.EventsSinceInclusive(0.15);
	BOOST_CHECK_EQUAL(events.Size(), 3);

	events = buffer.EventsSinceInclusive(0.2);
	BOOST_CHECK_EQUAL(events.Size(), 2);

	events = buffer.EventsSinceInclusive(0.25);
	BOOST_CHECK_EQUAL(events.Size(), 2);

	events = buffer.EventsSinceInclusive(0.3);
	BOOST_CHECK_EQUAL(events.Size(), 1);

	events = buffer.EventsSinceInclusive(0.35);
	BOOST_CHECK_EQUAL(events.Size(), 1);
}

BOOST_AUTO_TEST_CASE(EventsBetweenEmpty)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	auto events = buffer.EventsBetween(0.0, 0.0);
	BOOST_CHECK_EQUAL(events.Size(), 0);

	events = buffer.EventsBetween(0.0, 0.05);
	BOOST_CHECK_EQUAL(events.Size(), 0);

	events = buffer.EventsBetween(0.15, 0.18);
	BOOST_CHECK_EQUAL(events.Size(), 0);

	events = buffer.EventsBetween(0.31, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 0);
}


BOOST_AUTO_TEST_CASE(EventsBetweenUpper)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	auto events = buffer.EventsBetween(0.0, 0.1);
	BOOST_CHECK_EQUAL(events.Size(), 1);

	events = buffer.EventsBetween(0.0, 0.15);
	BOOST_CHECK_EQUAL(events.Size(), 1);

	events = buffer.EventsBetween(0.0, 0.2);
	BOOST_CHECK_EQUAL(events.Size(), 2);

	events = buffer.EventsBetween(0.0, 0.3);
	BOOST_CHECK_EQUAL(events.Size(), 3);

	events = buffer.EventsBetween(0.0, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 3);
}

BOOST_AUTO_TEST_CASE(EventsBetweenLower)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	auto events = buffer.EventsBetween(0.0, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 3);

	events = buffer.EventsBetween(0.05, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 3);

	events = buffer.EventsBetween(0.1, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 3);

	events = buffer.EventsBetween(0.2, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 2);

	events = buffer.EventsBetween(0.25, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 1);

	events = buffer.EventsBetween(0.3, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 1);

	events = buffer.EventsBetween(0.35, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 0);
}

BOOST_AUTO_TEST_CASE(EventsBetweenInclusiveEmpty)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	auto events = buffer.EventsBetweenInclusive(0.0, 0.0);
	BOOST_CHECK_EQUAL(events.Size(), 0);

	events = buffer.EventsBetweenInclusive(0.05, 0.1);
	BOOST_CHECK_EQUAL(events.Size(), 0);

	events = buffer.EventsBetweenInclusive(0.05, 0.25);
	BOOST_CHECK_EQUAL(events.Size(), 0);
}

BOOST_AUTO_TEST_CASE(EventsBetweenInclusiveLower)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	auto events = buffer.EventsBetweenInclusive(0.1, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 3);

	events = buffer.EventsBetweenInclusive(0.15, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 3);

	events = buffer.EventsBetweenInclusive(0.2, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 2);

	events = buffer.EventsBetweenInclusive(0.25, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 2);

	events = buffer.EventsBetweenInclusive(0.3, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 1);

	events = buffer.EventsBetweenInclusive(0.35, 0.4);
	BOOST_CHECK_EQUAL(events.Size(), 1);
}

BOOST_AUTO_TEST_CASE(EventsBetweenInclusiveUpper)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	auto events = buffer.EventsBetweenInclusive(0.1, 0.1);
	BOOST_CHECK_EQUAL(events.Size(), 1);

	events = buffer.EventsBetweenInclusive(0.1, 0.15);
	BOOST_CHECK_EQUAL(events.Size(), 1);

	events = buffer.EventsBetweenInclusive(0.1, 0.2);
	BOOST_CHECK_EQUAL(events.Size(), 2);

	events = buffer.EventsBetweenInclusive(0.1, 0.25);
	BOOST_CHECK_EQUAL(events.Size(), 2);

	events = buffer.EventsBetweenInclusive(0.1, 0.3);
	BOOST_CHECK_EQUAL(events.Size(), 3);

	events = buffer.EventsBetweenInclusive(0.1, 0.35);
	BOOST_CHECK_EQUAL(events.Size(), 3);
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
}

BOOST_AUTO_TEST_CASE(ContainsDataAfterOverwrite)
{
	EventBuffer<int, double> buffer(3);
	buffer.RegisterEvent(0.1, 1);
	buffer.RegisterEvent(0.2, 2);
	buffer.RegisterEvent(0.3, 3);

	// This one should overwrite
	buffer.RegisterEvent(0.4, 4);
	BOOST_CHECK(!buffer.ContainsDataAfter(0.1));
	BOOST_CHECK(buffer.ContainsDataAfter(0.2));
	BOOST_CHECK(buffer.ContainsDataAfter(0.4));
	BOOST_CHECK(!buffer.ContainsDataAfter(0.5));
}

BOOST_AUTO_TEST_CASE(LastTimestamp)
{
	EventBuffer<int, int> buffer(2);
	buffer.RegisterEvent(1, 1);

	BOOST_CHECK_EQUAL(buffer.AllEvents().LastTimestamp(), 1);

	buffer.RegisterEvent(2, 2);
	BOOST_CHECK_EQUAL(buffer.AllEvents().LastTimestamp(), 2);

	buffer.RegisterEvent(3, 3);
	BOOST_CHECK_EQUAL(buffer.AllEvents().LastTimestamp(), 3);

	buffer.RegisterEvent(4, 4);
	BOOST_CHECK_EQUAL(buffer.AllEvents().LastTimestamp(), 4);
}

BOOST_AUTO_TEST_CASE(IndexedDataAccess)
{
	EventBuffer<int, int> buffer(3);
	buffer.RegisterEvent(0, 1);
	buffer.RegisterEvent(1, 2);

	auto events = buffer.AllEvents();

	BOOST_CHECK_EQUAL(events[0].timestamp, 0);
	BOOST_CHECK_EQUAL(events[0].data, 1);

	BOOST_CHECK_EQUAL(events[1].timestamp, 1);
	BOOST_CHECK_EQUAL(events[1].data, 2);
}

BOOST_AUTO_TEST_CASE(ForEach)
{
	EventBuffer<int, int> buffer(3);
	buffer.RegisterEvent(0, 1);
	buffer.RegisterEvent(1, 2);

	auto events = buffer.AllEvents();

	int tsSum = 0;
	int dataSum = 0;

	events.ForEach([&tsSum, &dataSum](int const & ts, int const & data)
	{
		tsSum += ts;
		dataSum += data;
	});

	BOOST_CHECK_EQUAL(tsSum, 1);
	BOOST_CHECK_EQUAL(dataSum, 3);
}

BOOST_AUTO_TEST_CASE(ForEachWhile)
{
	EventBuffer<int, int> buffer(3);
	buffer.RegisterEvent(0, 1);
	buffer.RegisterEvent(1, 2);
	buffer.RegisterEvent(2, 3);

	auto events = buffer.AllEvents();

	int tsSum = 0;
	int dataSum = 0;

	events.ForEachWhile([&tsSum, &dataSum](int const & ts, int const & data) -> bool
	{
		tsSum += ts;
		dataSum += data;
		return (dataSum < 3);
	});

	BOOST_CHECK_EQUAL(tsSum, 1);
	BOOST_CHECK_EQUAL(dataSum, 3);
}

BOOST_AUTO_TEST_SUITE_END()