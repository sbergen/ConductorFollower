#pragma once

#include <boost/preprocessor.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/key_of.hpp>

/* Internal pre-processor stuff */

#define CF_OPTION_GROUP_select(_i) \
	BOOST_PP_IIF(BOOST_PP_EQUAL(BOOST_PP_MOD(_i, 3), 0), first, \
		BOOST_PP_IIF(BOOST_PP_EQUAL(BOOST_PP_MOD(_i, 3), 1), second, third))

#define CF_OPTION_GROUP_struct_i(_r, _data, _i, _elem) \
	BOOST_PP_CAT(CF_OPTION_GROUP_struct_, CF_OPTION_GROUP_select(_i))(_elem)
#define CF_OPTION_GROUP_struct_first(_name) struct _name 
#define CF_OPTION_GROUP_struct_second(_description) { static std::string description() { return _description; } };
#define CF_OPTION_GROUP_struct_third(_unused) 

#define CF_OPTION_GROUP_map_i(_r, _data, _i, _elem) \
	BOOST_PP_CAT(CF_OPTION_GROUP_map_, CF_OPTION_GROUP_select(_i))(_i, _elem)
#define CF_OPTION_GROUP_map_first(_i, _type) BOOST_PP_COMMA_IF(_i) boost::fusion::pair<_type, 
#define CF_OPTION_GROUP_map_second(_i, _description) 
#define CF_OPTION_GROUP_map_third(_i, _optionType) _optionType>

/**** Use only stuff below! ****/

namespace cf {
namespace Options {

template<typename MapType>
class OptionGroup
{
public:
	template<typename OptionType, typename ValueType>
	void SetValue(ValueType const & value)
	{
		boost::fusion::at_key<OptionType>(map).setValue(value);
	}

	template<typename OptionType, typename ValueType>
	void GetValue(ValueType & result)
	{
		result = boost::fusion::at_key<OptionType>(map).value();
	}

	// Calls F::operator(string, OptionType) for each option
	template<typename Functor>
	void ForEach(Functor f)
	{
		boost::fusion::for_each(map, ForeachHelper<Functor>(f));
	}

private:
	MapType map;

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
};

} // namespace Options
} // namespace cf

#define CF_OPTION_GROUP(_group, _seq) \
	BOOST_PP_SEQ_FOR_EACH_I(CF_OPTION_GROUP_struct_i, _group, _seq) \
	class _group : public cf::Options::OptionGroup< boost::fusion::map< BOOST_PP_SEQ_FOR_EACH_I(CF_OPTION_GROUP_map_i, _group, _seq) > > {};

