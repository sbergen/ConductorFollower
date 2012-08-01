#include <boost/test/unit_test.hpp>

#include "cf/rcu.h"

using namespace cf;

BOOST_AUTO_TEST_SUITE(RCUTests)

BOOST_AUTO_TEST_CASE(TestNonRTWrite)
{
	RTReadRCU<int> rcu(boost::make_shared<int>(0));

	auto reader = rcu.read();
	BOOST_CHECK_EQUAL(*reader, 0);

	{
		// Update via reader
		auto writer = rcu.writer();
		*writer = 1;

		// Reader should not see change
		BOOST_CHECK_EQUAL(*reader, 0);

		// Even and updated reader shouldn't yet
		reader = rcu.read();
		BOOST_CHECK_EQUAL(*reader, 0);
	}

	// Reader should still not see change
	BOOST_CHECK_EQUAL(*reader, 0);

	// Updated reader now should see it
	reader = rcu.read();
	BOOST_CHECK_EQUAL(*reader, 1);
}

BOOST_AUTO_TEST_CASE(TestRTWrite)
{
	RTWriteRCU<int> rcu(boost::make_shared<int>(0));

	auto reader = rcu.read();
	BOOST_CHECK_EQUAL(*reader, 0);

	{
		// Update via reader
		auto writer = rcu.writer();
		*writer = 1;

		// Reader should not see change
		BOOST_CHECK_EQUAL(*reader, 0);

		// Even and updated reader shouldn't yet
		reader = rcu.read();
		BOOST_CHECK_EQUAL(*reader, 0);
	}

	// Reader should still not see change
	BOOST_CHECK_EQUAL(*reader, 0);

	// Updated reader now should see it
	reader = rcu.read();
	BOOST_CHECK_EQUAL(*reader, 1);
}

template<typename T>
class Foo
{
public:
        template<typename Y>
        Foo & operator=(Y const & y)
        {
                val = y;
                std::cout << "template" << std::endl;
                return *this;
        }
 
        operator T() const { return val; }
 
        Foo & operator=(Foo const & other)
        {
                val = other.val;
                std::cout << "non template" << std::endl;
                return *this;
        }
 
 
private:
        T val;
};
 
BOOST_AUTO_TEST_CASE(AssignTest)
{
        Foo<int> f1;
        Foo<int> f2;
 
        f1 = 1;
        f2 = f1;
}

BOOST_AUTO_TEST_SUITE_END()