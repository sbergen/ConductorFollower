#include <boost/test/unit_test.hpp>

#include "cf/LockfreeRefCount.h"

using namespace cf;

BOOST_AUTO_TEST_SUITE(RefCountTests)

class TestClass : public LockfreeRefCountProvider
{
public:
	typedef LockfreeRefCounted<TestClass, bool> Counted;

	TestClass()
		: LockfreeRefCountProvider()
		, isReset_(false)
	{}

	Counted MakeCounted() { return Counted(*this, &TestClass::reset, true); }

	void reset(bool reset) { isReset_ = reset; }
	bool isReset() const { return isReset_; }

private:
	bool isReset_;
};

BOOST_AUTO_TEST_CASE(TestReturnFromFunc)
{
	TestClass provider;
	{
		auto counted = provider.MakeCounted();
		BOOST_CHECK(!provider.isReset());
	}
	BOOST_CHECK(provider.isReset());
}

BOOST_AUTO_TEST_CASE(TestCopy)
{
	TestClass provider;

	{
		auto counted = provider.MakeCounted();
		BOOST_CHECK(!provider.isReset());

		{
			TestClass::Counted counted2 = counted;
			BOOST_CHECK(!provider.isReset());
		}

		BOOST_CHECK(!provider.isReset());
	}
	BOOST_CHECK(provider.isReset());
}

BOOST_AUTO_TEST_SUITE_END()