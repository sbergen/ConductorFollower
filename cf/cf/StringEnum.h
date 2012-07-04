#pragma once

#include <boost/preprocessor.hpp>

#define CF_STRING_ENUM_visit(_r, _data, _i, _elem) \
	BOOST_PP_CAT(CF_STRING_ENUM_, _data)(_i, BOOST_PP_TUPLE_ELEM(0, _elem), BOOST_PP_TUPLE_ELEM(1, _elem))

#define CF_STRING_ENUM_enum_value(_i, _val, _str) BOOST_PP_COMMA_IF(_i) _val
#define CF_STRING_ENUM_conversion_case(_i, _val, _str) case _val: return _str;

#define CF_STRING_ENUM_enum_values(_seq) \
	BOOST_PP_SEQ_FOR_EACH_I(CF_STRING_ENUM_visit, enum_value, _seq)

#define CF_STRING_ENUM_conversion_cases(_seq) \
	BOOST_PP_SEQ_FOR_EACH_I(CF_STRING_ENUM_visit, conversion_case, _seq)

#define CF_STRING_ENUM_first_value(_seq) \
	BOOST_PP_TUPLE_ELEM(0, BOOST_PP_SEQ_HEAD(_seq))

#define CF_STRING_ENUM(_className, _seq) \
	class _className \
	{ \
	public: \
		enum Value { CF_STRING_ENUM_enum_values(_seq) }; \
		BOOST_STATIC_CONSTANT(unsigned, size = BOOST_PP_SEQ_SIZE(_seq) ); \
		_className () : value_( CF_STRING_ENUM_first_value(_seq) ) { } \
		_className (Value val) : value_(val) { } \
		_className & operator=(Value val) { value_ = val; return *this; } \
		operator char const *() const { switch(value_) { CF_STRING_ENUM_conversion_cases(_seq)  } } \
		operator Value() const { return value_; } \
	private: \
		Value value_; \
	};
	
