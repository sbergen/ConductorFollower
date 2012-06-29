#pragma once

#include <boost/fusion/include/key_of.hpp>
#include <boost/fusion/include/at_key.hpp>

#include "cf/FusionMap.h"

/*
define an option group like this:

typedef Option<int, 42, 0, 100> TestOptionType1;
typedef Option<int, 50, 0, 100> TestOptionType2;
typedef EnumOption<TestOptionEnum3, TestOptionEnum3::Second> TestOptionType3;

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

#define CF_OPTION_GROUP(_group, _seq) CF_FUSION_MAP(cf::OptionGroup, _group, _seq)

namespace cf {

// Mapped values should derive from cf::Option<...>
template<typename MapType>
class OptionGroup : public FusionMapBase<MapType>
{
public:
	template<typename OptionType, typename ValueType>
	void SetValue(ValueType const & value)
	{
		boost::fusion::at_key<OptionType>(map()) = value;
	}

	template<typename OptionType, typename ValueType>
	void GetValue(ValueType & result) const
	{
		result = static_cast<ValueType>(boost::fusion::at_key<OptionType>(map()));
	}
};

} // namespace cf

