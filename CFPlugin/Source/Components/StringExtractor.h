#include <boost/utility/enable_if.hpp>
#include <boost/mpl/has_xxx.hpp>

namespace juce { class String; }

BOOST_MPL_HAS_XXX_TRAIT_DEF(value_type)

// Recursion end point
template<typename T, typename Enable = void>
struct StringExtractor
{
	juce::String operator()(T const & val)
	{
		return String(val);
	}
};

// recursive value_type extraction
template<typename T>
struct StringExtractor<T, typename boost::enable_if<has_value_type<T> >::type>
{
	typedef typename T::value_type value_type;

	juce::String operator()(T const & val)
	{
		return StringExtractor<value_type>()(static_cast<value_type>(val));
	}
};