#pragma once

#include <boost/fusion/include/key_of.hpp>
#include <boost/fusion/include/at_key.hpp>

#include "cf/FusionMap.h"

/*
define an option group like this:

typedef Options::Option<int, 42, 0, 100> TestOptionType1;
typedef Options::Option<int, 50, 0, 100> TestOptionType2;
typedef Options::EnumOption<TestOptionEnum3, TestOptionEnum3::Second> TestOptionType3;

CF_OPTION_GROUP(TestOptionGroup,
	(Option1)("This is the option number 1")(TestOptionType1)
	(Option2)("This is the option number 2")(TestOptionType2)
	(Option3)("This is the option number 3")(TestOptionType3)
)

and then use it like this:

TestOptionGroup group;
int value;
group.GetValue<Option1>(value);
group.SetValue<Option1>(50);

*/

#define CF_OPTION_GROUP(_group, _seq) CF_FUSION_MAP(cf::Options::OptionGroup, _group, _seq)

namespace cf {
namespace Options {

template<typename MapType>
class OptionGroup
{
public:
	template<typename OptionType, typename ValueType>
	void SetValue(ValueType const & value)
	{
		boost::fusion::at_key<OptionType>(map).setValue(value);
	}

	template<typename OptionType, typename ValueType>
	void GetValue(ValueType & result)
	{
		result = boost::fusion::at_key<OptionType>(map).value();
	}

	// Calls F::operator(string, OptionType) for each option
	template<typename Functor>
	void ForEach(Functor f)
	{
		boost::fusion::for_each(map, ForeachHelper<Functor>(f));
	}

private:
	MapType map;

	template<typename F>
	struct ForeachHelper
	{
		ForeachHelper(F & f) : f(f) {}
		F & f;

		template<typename PairType>
		void operator()(PairType const & pair) const
		{
			typedef PairType::first_type keyType;
			f(keyType::description(), pair.second);
		}
	};
};

} // namespace Options
} // namespace cf

