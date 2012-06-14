#include "cf/Options.h"
#include "cf/OptionGroup.h"

#include <iostream>

#include <boost/enum.hpp>
#include <boost/fusion/include/for_each.hpp>

using namespace cf;

BOOST_ENUM_VALUES(TestOptionEnum3, std::string,
	(First)("First")
	(Second)("Second")
	(Third)("Third")
)

typedef Option<int, 42, 0, 100> TestOptionType;
typedef Option<int, 50, 0, 100> TestOptionType2;
typedef EnumOption<TestOptionEnum3, TestOptionEnum3::Second> TestOptionType3;

CF_OPTION_GROUP(TestOptionGroup,
	(Option1)("This is the option number 1")(TestOptionType)
	(Option2)("This is the option number 2")(TestOptionType2)
	(Option3)("This is the option number 3")(TestOptionType3)
)

BOOST_AUTO_TEST_SUITE(OptionGroupTests)

BOOST_AUTO_TEST_CASE(BasicTest)
{
	TestOptionGroup group;
	int value;
	group.GetValue<Option1>(value);
	BOOST_CHECK_EQUAL(value, 42);
	group.SetValue<Option1>(50);
	group.GetValue<Option1>(value);
	BOOST_CHECK_EQUAL(value, 50);
}

class OptionPrinter
{
public:
	template<typename T>
	void operator()(std::string const & desc, T const & t)
	{
		std::ostream stream(0);
		typename T::value_type maxValue = static_cast<typename T::assignable_type>(T::max_value);
		stream << desc << ": " << t.value()
			<< ", max: " << maxValue
			<< ", is enum: " << T::is_enum::value
			<< std::endl;
	}
};

BOOST_AUTO_TEST_CASE(ForeachTest)
{
	TestOptionGroup group;
	group.ForEach(OptionPrinter());
}

BOOST_AUTO_TEST_SUITE_END()