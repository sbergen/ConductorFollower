#pragma once

#include <boost/fusion/include/map.hpp>
#include <boost/serialization/access.hpp>

#include "cf/FusionMapMacros.h"

// Main macro for defining the map
#define CF_FUSION_MAP(_baseClass, _group, _seq) \
	BOOST_PP_SEQ_FOR_EACH_I(CF_FUSION_MAP_VISIT, struct, _seq) \
	class _group : public _baseClass< boost::fusion::map< BOOST_PP_SEQ_FOR_EACH_I(CF_FUSION_MAP_VISIT, map, _seq) > > \
	{ CF_FUSION_MAP_transformed(_seq) };

namespace cf {

template<typename T> class PairArchiver;

template<typename MapType>
class FusionMapBase
{
public:
	// Calls F::operator(string, ValueType) for each option
	template<typename Functor>
	void ForEach(Functor f)
	{
		boost::fusion::for_each(map_, ForeachHelper<Functor>(f));
	}

	MapType const & map() const { return map_; }
	MapType & map() { return map_; }

protected:
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

private:
	MapType map_;

private: // Serialization
	friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int /*version*/)
    {
        boost::fusion::for_each(map_, PairArchiver<Archive>(ar));
    }
};

} // namespace cf