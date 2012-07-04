#include <boost/test/unit_test.hpp>

#include "cf/StringEnum.h"

BOOST_AUTO_TEST_SUITE(StringEnumTests)

CF_STRING_ENUM(TestEnum,
	((First, "First"))
	((Second, "Second"))
)

BOOST_AUTO_TEST_CASE(TestConstruction)
{
	// default
	TestEnum e1;
	BOOST_CHECK_EQUAL(e1, TestEnum::First);

	// from value
	TestEnum e2(TestEnum::Second);
	BOOST_CHECK_EQUAL(e2, TestEnum::Second);

	// copy
	TestEnum e3(e2);
	BOOST_CHECK_EQUAL(e3, TestEnum::Second);
}

BOOST_AUTO_TEST_CASE(TestAssignment)
{
	TestEnum e1;
	TestEnum e2(TestEnum::Second);

	// Direct
	BOOST_CHECK_NE(e1, TestEnum::Second);
	e1 = e2;
	BOOST_CHECK_EQUAL(e1, TestEnum::Second);

	// from value
	BOOST_CHECK_NE(e1, TestEnum::First);
	e1 = TestEnum::First;
	BOOST_CHECK_EQUAL(e1, TestEnum::First);
}

BOOST_AUTO_TEST_CASE(TestConversion)
{
	TestEnum e(TestEnum::Second);

	// String
	BOOST_CHECK_EQUAL(std::string(e), std::string("Second"));

	// Value
	BOOST_CHECK_EQUAL(e, TestEnum::Second);
}

BOOST_AUTO_TEST_CASE(TestSize)
{
	BOOST_CHECK_EQUAL(TestEnum::size, 2);
}

BOOST_AUTO_TEST_CASE(TestCompare)
{
	TestEnum e1(TestEnum::First);
	TestEnum e2(TestEnum::Second);

	BOOST_CHECK(e1 == e1);
	BOOST_CHECK(e1 != e2);

	BOOST_CHECK(e1 == TestEnum::First);
	BOOST_CHECK(e1 != TestEnum::Second);
}

BOOST_AUTO_TEST_SUITE_END()