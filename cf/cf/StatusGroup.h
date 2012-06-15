#pragma once

#include <boost/fusion/include/key_of.hpp>
#include <boost/fusion/include/at_key.hpp>

#include "cf/FusionMap.h"
#include "cf/StatusItem.h"

// Customized macros for map item generation
#define CF_STATUS_GROUP_make_value_type(_group, _valueType) \
	typename _valueType::grouped<_group>::type 

/*
Use this like:
CF_STATUS_GROUP(TestStatGroup,
	(Stat1)("This is the stat number 1")(int)
	(Stat2)("This is the stat number 2")(float)
)
*/

#define CF_STATUS_GROUP(_group, _seq) CF_FUSION_MAP_CUSTOM_T(cf::StatusGroup, _group, _group, _seq, CF_STATUS_GROUP_make_value_type)

namespace cf {

// Mapped values should derive from cf::ChangeTracked<...>
template<typename DerivedType, typename MapType>
class StatusGroup : public FusionMapBase<MapType>
{
public:
	template<typename OptionType, typename ValueType>
	void SetValue(ValueType const & value)
	{
		boost::fusion::at_key<OptionType>(map()).setValue(value);
	}

	template<typename OptionType, typename ValueType>
	void GetValue(ValueType & result) const
	{
		result = boost::fusion::at_key<OptionType>(map()).value();
	}

	template<typename OptionType, typename ValueType>
	bool LoadIfChanged(ValueType & result) const
	{
		return boost::fusion::at_key<OptionType>(map()).LoadIfChanged(result);
	}

	bool HasSomethingChanged(bool reset = true)
	{
		return ChangeTrackedGroup<DerivedType>::HasSomethingChanged(reset);
	}
};

} // namespace cf

