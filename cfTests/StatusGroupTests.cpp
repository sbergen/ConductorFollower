#include <boost/test/unit_test.hpp>

#include "cf/StatusGroup.h"

#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/fusion/include/for_each.hpp>

using namespace cf;

typedef LimitedStatusItem<Status::Setting, Status::Text, int, 42, 0, 100> IntStatus;
typedef FloatStatusItem<Status::Setting, Status::Bar, 5, 0, 10> FloatStatus;

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

BOOST_AUTO_TEST_CASE(AtTest)
{
	TestStatGroup group;
	auto & s1 = group.at<Stat1>();
}

BOOST_AUTO_TEST_CASE(TransformTest)
{
	TestStatGroup group;
	
	typedef TestStatGroup::transformed<OtherType>::type trans_type;
	trans_type transformed;
	Transformer<trans_type> t(transformed);
	boost::fusion::for_each(group.map(), t);
}

BOOST_AUTO_TEST_CASE(TestCopying)
{
	TestStatGroup group;
	TestStatGroup group2;

	int i, i2;

	group2.at<Stat1>() = 37;

	i = group.at<Stat1>();
	i2 = group2.at<Stat1>();
	BOOST_CHECK_EQUAL(i, 42);
	BOOST_CHECK_EQUAL(i2, 37);

	// Assignment
	group2 = group;
	i2 = group2.at<Stat1>();
	BOOST_CHECK_EQUAL(i2, 42);

	// Changes don't affect copies
	group.at<Stat1>() = 37;
	group2.at<Stat1>() = i2;
	BOOST_CHECK_EQUAL(i2, 42);
}

BOOST_AUTO_TEST_SUITE_END()