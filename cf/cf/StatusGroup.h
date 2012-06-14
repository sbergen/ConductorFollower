#pragma once

#include <boost/fusion/include/key_of.hpp>
#include <boost/fusion/include/at_key.hpp>

#include "cf/FusionMap.h"
#include "cf/ChangeTracked.h"

#define CF_STATUS_GROUP(_group, _seq) CF_FUSION_MAP_T1(cf::StatusGroup, _group, _group, _seq)

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

