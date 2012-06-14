#pragma once

#include <boost/fusion/include/key_of.hpp>
#include <boost/fusion/include/at_key.hpp>

#include "cf/FusionMap.h"
#include "cf/ChangeTracked.h"

// Customized macros for map item generation
#define CF_STATUS_GROUP_make_value_type(_group, _valueType) \
	ChangeTracked<_group BOOST_PP_COMMA() _valueType> 
#define CF_FUSION_MAP_statusGroup_map0(_group, _i, _keyType) CF_FUSION_MAP_map0(_group, _i, _keyType)
#define CF_FUSION_MAP_statusGroup_map1(_group, _i, _description) CF_FUSION_MAP_map1(_group, _i, _description) 
#define CF_FUSION_MAP_statusGroup_map2(_group, _i, _valueType) CF_FUSION_MAP_map2(_group, _i, CF_STATUS_GROUP_make_value_type(_group, _valueType))

/*
Use this like:
CF_STATUS_GROUP(TestStatGroup,
	(Stat1)("This is the stat number 1")(int)
	(Stat2)("This is the stat number 2")(float)
)
*/

#define CF_STATUS_GROUP(_group, _seq) CF_FUSION_MAP_CUSTOM_T(cf::StatusGroup, _group, _group, _seq, struct, statusGroup_map)

namespace cf {

// Mapped values should derive from cf::ChangeTracked<...>
template<typename DerivedType, typename MapType>
class StatusGroup : public FusionMapBase<MapType>
{
public:
	template<typename OptionType, typename ValueType>
	void SetValue(ValueType const & value)
	{
		boost::fusion::at_key<OptionType>(map).Set(value);
	}

	template<typename OptionType, typename ValueType>
	bool LoadIfChanged(ValueType & result)
	{
		return boost::fusion::at_key<OptionType>(map).LoadIfChanged(result);
	}

	bool HasSomethingChanged(bool reset = true)
	{
		return ChangeTrackedGroup<DerivedType>::HasSomethingChanged(reset);
	}
};

} // namespace cf

