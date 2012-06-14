#pragma once

#include <boost/preprocessor.hpp>
#include <boost/fusion/include/map.hpp>

// A bunch of macros for creating a boost::fusion::map with custom key types.
// The keys have a static descriptio() function, that returns a description.
// The values can be of any type.

// Macros for building the option structs (indexing and description)
#define CF_FUSION_MAP_struct_i(_r, _data, _i, _elem) \
	BOOST_PP_CAT(CF_FUSION_MAP_struct_, BOOST_PP_MOD(_i, 3))(_elem)
#define CF_FUSION_MAP_struct_0(_type) struct _type 
#define CF_FUSION_MAP_struct_1(_description) { static std::string description() { return _description; } };
#define CF_FUSION_MAP_struct_2(_optionType) 

// Macros for building the fusion::map
#define CF_FUSION_MAP_map_i(_r, _data, _i, _elem) \
	BOOST_PP_CAT(CF_FUSION_MAP_map_, BOOST_PP_MOD(_i, 3))(_i, _elem)
#define CF_FUSION_MAP_map_0(_i, _type) BOOST_PP_COMMA_IF(_i) boost::fusion::pair<_type, 
#define CF_FUSION_MAP_map_1(_i, _description) 
#define CF_FUSION_MAP_map_2(_i, _optionType) _optionType>

// Main macro for defining the map
#define CF_FUSION_MAP(_baseClass, _group, _seq) \
	BOOST_PP_SEQ_FOR_EACH_I(CF_FUSION_MAP_struct_i, _group, _seq) \
	class _group : public _baseClass< boost::fusion::map< BOOST_PP_SEQ_FOR_EACH_I(CF_FUSION_MAP_map_i, _group, _seq) > > {};

