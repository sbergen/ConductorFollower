#include "cf/StatusGroup.h"

using namespace cf;

class TestStatGroup;

CF_STATUS_GROUP(TestStatGroup,
	(Stat1)("This is the stat number 1")(int)
	(Stat2)("This is the stat number 2")(float)
)

BOOST_AUTO_TEST_SUITE(StatusGroupTests)

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


BOOST_AUTO_TEST_SUITE_END()