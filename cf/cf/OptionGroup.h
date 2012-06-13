#pragma once

#include <boost/preprocessor.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/key_of.hpp>

/* Internal pre-processor stuff */

// Macros for building the option structs (indexing and description)
#define CF_OPTION_GROUP_struct_i(_r, _data, _i, _elem) \
	BOOST_PP_CAT(CF_OPTION_GROUP_struct_, BOOST_PP_MOD(_i, 3))(_elem)
#define CF_OPTION_GROUP_struct_0(_type) struct _type 
#define CF_OPTION_GROUP_struct_1(_description) { static std::string description() { return _description; } };
#define CF_OPTION_GROUP_struct_2(_optionType) 

// Macros for building the fusion::map
#define CF_OPTION_GROUP_map_i(_r, _data, _i, _elem) \
	BOOST_PP_CAT(CF_OPTION_GROUP_map_, BOOST_PP_MOD(_i, 3))(_i, _elem)
#define CF_OPTION_GROUP_map_0(_i, _type) BOOST_PP_COMMA_IF(_i) boost::fusion::pair<_type, 
#define CF_OPTION_GROUP_map_1(_i, _description) 
#define CF_OPTION_GROUP_map_2(_i, _optionType) _optionType>

/**** Use only stuff below! ****/

/*

define an option group like this:

typedef Options::Option<int, 42, 0, 100> TestOptionType1;
typedef Options::Option<int, 50, 0, 100> TestOptionType2;
typedef Options::EnumOption<TestOptionEnum3, TestOptionEnum3::Second> TestOptionType3;

CF_OPTION_GROUP(TestOptionGroup,
	(Option1)("This is the option number 1")(TestOptionType1)
	(Option2)("This is the option number 2")(TestOptionType2)
	(Option3)("This is the option number 3")(TestOptionType3)
)

and then use it like this:

TestOptionGroup group;
int value;
group.GetValue<Option1>(value);
group.SetValue<Option1>(50);

*/
#define CF_OPTION_GROUP(_group, _seq) \
	BOOST_PP_SEQ_FOR_EACH_I(CF_OPTION_GROUP_struct_i, _group, _seq) \
	class _group : public cf::Options::OptionGroup< boost::fusion::map< BOOST_PP_SEQ_FOR_EACH_I(CF_OPTION_GROUP_map_i, _group, _seq) > > {};

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

