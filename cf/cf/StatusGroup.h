#pragma once

#include <boost/fusion/include/key_of.hpp>
#include <boost/fusion/include/at_key.hpp>

#include "cf/FusionMap.h"
#include "cf/StatusItem.h"

// Couldn't figure out how to do this with C++11 stuff,
// at least not in VS2010 :(
#define VAL_AT(TMap, TOption) boost::fusion::at_key<TOption>(*((TMap *)nullptr))
#define CONST_VAL_AT(TMap, TOption) (VAL_AT(TMap const, TOption))

/*
Use this like:
CF_STATUS_GROUP(TestStatGroup,
	((Stat1, "This is the stat number 1", int))
	(Stat2, "This is the stat number 2", float)
)
*/

#define CF_STATUS_GROUP(_group, _seq) CF_FUSION_MAP(cf::StatusGroup, _group, _seq)

namespace cf {

template<typename MapType>
class StatusGroup : public FusionMapBase<MapType>
{
public:
	template<typename OptionType>
	auto at() -> decltype(VAL_AT(MapType, OptionType))
	{
		return boost::fusion::at_key<OptionType>(map());
	}

	template<typename OptionType>
	auto at() const -> decltype(CONST_VAL_AT(MapType, OptionType))
	{
		return boost::fusion::at_key<OptionType>(map());
	}
};

} // namespace cf

#undef VAL_AT
#undef CONST_VAL_AT
