#include "cf/Options.h"
#include "cf/OptionGroup.h"

#include <iostream>

#include <boost/enum.hpp>
#include <boost/fusion/include/for_each.hpp>

using namespace cf;

typedef Options::Option<int, 42, 0, 100> TestOptionType;
typedef Options::Option<int, 50, 0, 100> TestOptionType2;

CF_OPTION_GROUP(TestOptionGroup,
	(Option1)("This is the option number 1")(TestOptionType)
	(Option2)("This is the option number 2")(TestOptionType2)
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
	void operator()(std::string const & desc, T const & t) const
	{
		//std::cout << desc << ": " << t.value() << ", max: " << T::max_value << std::endl;
	}
};

BOOST_AUTO_TEST_CASE(ForeachTest)
{
	TestOptionGroup group;
	group.ForEach(OptionPrinter());
}

BOOST_AUTO_TEST_SUITE_END()