#include <boost/test/unit_test.hpp>

#include "cf/StatusGroup.h"

#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/fusion/include/for_each.hpp>

using namespace cf;

typedef LimitedStatusItem<StatusType::Setting, StatusPresentation::Text, int, 42, 0, 100> IntStatus;
typedef FloatStatusItem<StatusType::Setting, StatusPresentation::Bar, 5, 0, 10> FloatStatus;

CF_STATUS_GROUP(TestStatGroup,
	((Stat1, "This is the stat number 1", IntStatus))
	((Stat2, "This is the stat number 2", FloatStatus))
)

using namespace boost::fusion;

BOOST_AUTO_TEST_SUITE(StatusGroupTests)

template<typename KeyType, typename ValueType>
struct OtherType
{
	void f(ValueType const & v) { auto val = static_cast<ValueType::value_type>(v); }
};

template<typename T>
struct Transformer
{
	Transformer(T & t) : t(t) {}

	template<typename PairType>
	void operator()(PairType const & pair) const
	{
		at_key<typename PairType::first_type>(t).f(pair.second);
	}

	mutable T & t;
};

BOOST_AUTO_TEST_CASE(TransformTest)
{
	TestStatGroup group;
	
	typedef TestStatGroup::transformed<OtherType>::type trans_type;
	trans_type transformed;
	Transformer<trans_type> t(transformed);
	boost::fusion::for_each(group.map(), t);
}

BOOST_AUTO_TEST_SUITE_END()