#include <boost/preprocessor.hpp>

// A bunch of macros for creating a boost::fusion::map with custom key types.
// The keys have a static description() function, that returns a description.
// The values can be of any type.

// Selection macro for foreach
#define CF_FUSION_MAP_VISIT(_r, _type, _i, _elem) \
	BOOST_PP_CAT(CF_FUSION_MAP_element_, _type)(_i, BOOST_PP_TUPLE_ELEM(0, _elem), BOOST_PP_TUPLE_ELEM(1, _elem), BOOST_PP_TUPLE_ELEM(2, _elem))

// Macro for building the option structs (indexing and description)
#define CF_FUSION_MAP_element_struct(_i, _keyType, _description, _valueType) \
	struct _keyType { static std::string description() { return _description; } };

// Macro for building the fusion::map
#define CF_FUSION_MAP_element_map(_i, _keyType, _description, _valueType) \
	BOOST_PP_COMMA_IF(_i) boost::fusion::pair<_keyType, _valueType >

// Macros for building transformations
#define CF_FUSION_MAP_transformed(_seq) \
	public:  template<template<typename, typename> class T> struct transformed { \
	typedef boost::fusion::map< BOOST_PP_SEQ_FOR_EACH_I(CF_FUSION_MAP_VISIT, transformed, _seq) > type; \
	};
#define CF_FUSION_MAP_element_transformed(_i, _keyType, _description, _valueType) \
	BOOST_PP_COMMA_IF(_i) boost::fusion::pair<_keyType, T<_keyType, _valueType > >