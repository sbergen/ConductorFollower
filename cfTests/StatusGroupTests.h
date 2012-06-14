#include "cf/StatusGroup.h"

#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/fusion/include/for_each.hpp>

using namespace cf;

CF_STATUS_GROUP(TestStatGroup,
	(Stat1)("This is the stat number 1")(int)
	(Stat2)("This is the stat number 2")(float)
)

using namespace boost::fusion;

BOOST_AUTO_TEST_SUITE(StatusGroupTests)

template<typename KeyType, typename ValueType>
struct OtherType
{
	void f(ValueType const & v) { std::cout << v.value() << std::endl; }
	void foo() { std::cout << "Foo" << std::endl; }
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

BOOST_AUTO_TEST_CASE(CheckGlobalChanges)
{
	TestStatGroup group;
	BOOST_CHECK(!group.HasSomethingChanged());

	group.SetValue<Stat1>(42);
	BOOST_CHECK(group.HasSomethingChanged(false));
	BOOST_CHECK(group.HasSomethingChanged());
	BOOST_CHECK(!group.HasSomethingChanged());

	group.SetValue<Stat1>(42);
	BOOST_CHECK(!group.HasSomethingChanged());
}

BOOST_AUTO_TEST_CASE(TransformTest)
{
	TestStatGroup group;
	
	typedef TestStatGroup::transformed<OtherType>::type trans_type;
	trans_type transformed;
	Transformer<trans_type> t(transformed);
	boost::fusion::for_each(group.map(), t);


}

BOOST_AUTO_TEST_SUITE_END()