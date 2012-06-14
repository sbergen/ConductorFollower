#pragma once

#include <boost/preprocessor.hpp>
#include <boost/fusion/include/map.hpp>

// A bunch of macros for creating a boost::fusion::map with custom key types.
// The keys have a static descriptio() function, that returns a description.
// The values can be of any type.

#define CF_FUSION_MAP_make_visit_macro(_data, _i) \
	BOOST_PP_CAT(BOOST_PP_CAT(CF_FUSION_MAP_, BOOST_PP_TUPLE_ELEM(1, _data)), BOOST_PP_MOD(_i, 3))

// Selection macro for mod3 stuff
#define CF_FUSION_MAP_VISIT(_r, _data, _i, _elem) \
	CF_FUSION_MAP_make_visit_macro(_data, _i)(BOOST_PP_TUPLE_ELEM(0, _data), _i, _elem)

// Macros for building the option structs (indexing and description)
#define CF_FUSION_MAP_struct0(_group, _i, _keyType) struct _keyType 
#define CF_FUSION_MAP_struct1(_group, _i, _description) { static std::string description() { return _description; } };
#define CF_FUSION_MAP_struct2(_group, _i, _valueType) 

// Macros for building the fusion::map
#define CF_FUSION_MAP_map0(_group, _i, _keyType) BOOST_PP_COMMA_IF(_i) boost::fusion::pair<_keyType, 
#define CF_FUSION_MAP_map1(_group, _i, _description) 
#define CF_FUSION_MAP_map2(_group, _i, _valueType) _valueType >

// Macros for building transformations
#define CF_FUSION_MAP_transformed(_group, _seq) \
	public:  template<template<typename, typename> class T> struct transformed { \
	typedef boost::fusion::map< BOOST_PP_SEQ_FOR_EACH_I(CF_FUSION_MAP_VISIT, (_group, transformed), _seq) > type; \
	};
#define CF_FUSION_MAP_transformed0(_group, _i, _keyType) BOOST_PP_COMMA_IF(_i) boost::fusion::pair<_keyType, T<_keyType, 
#define CF_FUSION_MAP_transformed1(_group, _i, _description)
#define CF_FUSION_MAP_transformed2(_group, _i, _valueType) _valueType > >

// Intermediate layer, which may be customized
#define CF_FUSION_MAP_CUSTOM(_baseClass, _group, _seq, _struct, _map) \
	BOOST_PP_SEQ_FOR_EACH_I(CF_FUSION_MAP_VISIT, (_group, _struct), _seq) \
	class _group : public _baseClass< boost::fusion::map< BOOST_PP_SEQ_FOR_EACH_I(CF_FUSION_MAP_VISIT, (_group, _map), _seq) > > \
	{ CF_FUSION_MAP_transformed(_group, _seq) };

// Intermediate layer, which may be customized, for template parameters
#define CF_FUSION_MAP_CUSTOM_T(_baseClass, _t, _group, _seq, _struct, _map) \
	BOOST_PP_SEQ_FOR_EACH_I(CF_FUSION_MAP_VISIT, (_group, _struct), _seq) \
	class _group : public _baseClass<_t, boost::fusion::map< BOOST_PP_SEQ_FOR_EACH_I(CF_FUSION_MAP_VISIT, (_group, _map), _seq) > > \
	{ CF_FUSION_MAP_transformed(_group, _seq) };

// Main macros for defining the map, with default generators
#define CF_FUSION_MAP(_baseClass, _group, _seq) CF_FUSION_MAP_CUSTOM(_baseClass, _group, _seq, struct, map)
#define CF_FUSION_MAP_T(_baseClass, _t, _group, _seq) CF_FUSION_MAP_CUSTOM_T(_baseClass, _t, _group, _seq, struct, map)

namespace cf {

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
};

} // namespace cf