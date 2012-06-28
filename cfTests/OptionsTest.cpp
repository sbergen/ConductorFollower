#include <boost/test/unit_test.hpp>

#include "cf/Options.h"
#include "cf/OptionGroup.h"

#include <boost/enum.hpp>

using namespace cf;

BOOST_ENUM_VALUES(TestOption, std::string,
	(First)("First")
	(Second)("Second")
	(Third)("Third")
)

BOOST_AUTO_TEST_SUITE(OptionsTests)

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
	FloatOption<10, 0, 100> option;
	BOOST_CHECK_CLOSE(option.value(), 10.0, 0.1);

	option.setValue(1.5);
	BOOST_CHECK_CLOSE(option.value(), 1.5, 0.1);

	option.setValue(110);
	BOOST_CHECK_CLOSE(option.value(), 100.0, 0.1);

	option.setValue(-0.1);
	BOOST_CHECK_CLOSE(option.value(), 0.0, 0.1);
}

BOOST_AUTO_TEST_CASE(TestGroup)
{

}

BOOST_AUTO_TEST_SUITE_END()