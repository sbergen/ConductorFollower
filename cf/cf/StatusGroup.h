#pragma once

#include <boost/fusion/include/key_of.hpp>
#include <boost/fusion/include/at_key.hpp>

#include "cf/FusionMap.h"
#include "cf/StatusItem.h"

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
	auto at() -> decltype(boost::fusion::at_key<OptionType>(*((MapType *)nullptr)))
	{
		return boost::fusion::at_key<OptionType>(map());
	}

	template<typename OptionType>
	auto at() const -> decltype(boost::fusion::at_key<OptionType>(*((MapType const *)nullptr)))
	{
		return boost::fusion::at_key<OptionType>(map());
	}
};

} // namespace cf

