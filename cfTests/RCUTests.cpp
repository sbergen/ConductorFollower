#include <boost/test/unit_test.hpp>

#include <boost/make_shared.hpp>

#include "cf/rcu.h"
#include "cf/ButlerThread.h"
#include "cf/ButlerDeletable.h"

using namespace cf;

BOOST_AUTO_TEST_SUITE(RCUTests)

struct IntWrapper : public ButlerDeletable
{
	IntWrapper(int i) : i(i) {}
	int i;
};

template<typename RCUType>
void TestOneWrite(RCUType & rcu, int previousValue, int newValue)
{
	auto reader = rcu.read();
	BOOST_CHECK_EQUAL(reader->i, previousValue);

	{
		// Update via reader
		auto writer = rcu.writer();
		writer->i = newValue;

		// Reader should not see change
		BOOST_CHECK_EQUAL(reader->i, previousValue);

		// Even an updated reader shouldn't yet
		reader = rcu.read();
		BOOST_CHECK_EQUAL(reader->i, previousValue);
	}

	// Reader should still not see change
	BOOST_CHECK_EQUAL(reader->i, previousValue);

	// Updated reader now should see it
	reader = rcu.read();
	BOOST_CHECK_EQUAL(reader->i, newValue);
}

BOOST_AUTO_TEST_CASE(TestNonRTWrite)
{
	auto butler = boost::make_shared<ButlerThread>(milliseconds_t(10));
	RTReadRCU<IntWrapper> rcu(butler, IntWrapper(0));
	TestOneWrite(rcu, 0, 1);
}

BOOST_AUTO_TEST_CASE(TestRTWrite)
{
	auto butler = boost::make_shared<ButlerThread>(milliseconds_t(10));
	RTWriteRCU<IntWrapper> rcu(butler, IntWrapper(0));
	TestOneWrite(rcu, 0, 1);
}

BOOST_AUTO_TEST_CASE(TestContinuousRTWrite)
{
	auto butler = boost::make_shared<ButlerThread>(milliseconds_t(10));
	RTWriteRCU<IntWrapper> rcu(butler, IntWrapper(0));

	for (int i = 1; i < 10; ++i)
	{
		TestOneWrite(rcu, i - 1, i);
	}
}

BOOST_AUTO_TEST_CASE(TestContinuousNonRTWrite)
{
	auto butler = boost::make_shared<ButlerThread>(milliseconds_t(10));
	RTReadRCU<IntWrapper> rcu(butler, IntWrapper(0));

	for (int i = 1; i < 10; ++i)
	{
		TestOneWrite(rcu, i - 1, i);
	}
}

BOOST_AUTO_TEST_SUITE_END()