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
	BOOST_CHECK_EQUAL(option, 42);

	option = 50;
	BOOST_CHECK_EQUAL(option, 50);

	option = -1;
	BOOST_CHECK_EQUAL(option, 0);

	option = 150;
	BOOST_CHECK_EQUAL(option, 100);
}

BOOST_AUTO_TEST_CASE(EnumTest)
{
	EnumOption<TestOption, TestOption::Second> option;
	BOOST_CHECK_EQUAL(static_cast<TestOption>(option), TestOption::Second);

	option = TestOption::Third;
	BOOST_CHECK_EQUAL(static_cast<TestOption>(option), TestOption::Third);

	option = TestOption::First;
	BOOST_CHECK_EQUAL(static_cast<TestOption>(option), TestOption::First);
}

BOOST_AUTO_TEST_CASE(FloatTest)
{
	FloatOption<10, 0, 100> option;
	BOOST_CHECK_CLOSE(static_cast<double>(option), 10.0, 0.1);

	option = 1.5;
	BOOST_CHECK_CLOSE(static_cast<double>(option), 1.5, 0.1);

	option = 110;
	BOOST_CHECK_CLOSE(static_cast<double>(option), 100.0, 0.1);

	option = -0.1;
	BOOST_CHECK_CLOSE(static_cast<double>(option), 0.0, 0.1);
}

BOOST_AUTO_TEST_CASE(TestGroup)
{

}

BOOST_AUTO_TEST_SUITE_END()