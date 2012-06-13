#include "cf/Options.h"

#include <boost/enum.hpp>

using namespace cf;
using namespace cf::Options;

BOOST_AUTO_TEST_SUITE(OptionsTests)

BOOST_ENUM_VALUES(TestOption, std::string,
	(First)("First")
	(Second)("Second")
	(Third)("Third")
)

BOOST_AUTO_TEST_CASE(IntTest)
{
	Option<int, 42, 0, 100> option;
	BOOST_CHECK_EQUAL(option.value(), 42);

	option.setValue(50);
	BOOST_CHECK_EQUAL(option.value(), 50);

	option.setValue(-1);
	BOOST_CHECK_EQUAL(option.value(), 0);

	option.setValue(150);
	BOOST_CHECK_EQUAL(option.value(), 100);
}

BOOST_AUTO_TEST_CASE(EnumTest)
{
	EnumOption<TestOption, TestOption::Second> option;
	BOOST_CHECK_EQUAL(option.value(), TestOption::Second);

	option.setValue(TestOption::Third);
	BOOST_CHECK_EQUAL(option.value(), TestOption::Third);

	option.setValue(TestOption::First);
	BOOST_CHECK_EQUAL(option.value(), TestOption::First);
}

BOOST_AUTO_TEST_CASE(FloatTest)
{
	Option<int, 42, 0, 100> option;
	FloatProxy<Option<int, 42, 0, 100> > proxy(option, 0.0, 1.0);
	BOOST_CHECK_CLOSE(proxy.value(), 0.42, 1);

	proxy.setValue(0.7);
	BOOST_CHECK_CLOSE(proxy.value(), 0.7, 0.1);

	proxy.setValue(1.7);
	BOOST_CHECK_CLOSE(proxy.value(), 1.0, 0.1);

	proxy.setValue(-0.1);
	BOOST_CHECK_CLOSE(proxy.value(), 0.0, 0.1);
}

BOOST_AUTO_TEST_SUITE_END()