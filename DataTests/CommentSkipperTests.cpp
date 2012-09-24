#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "src/CommentSkipper.h"

BOOST_AUTO_TEST_SUITE(CommentSkipperTests)

using namespace cf;
using namespace cf::Data;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

typedef std::vector<std::string> StringList;

// Test grammar
template <typename Iterator>
struct TestGrammar : qi::grammar<Iterator, StringList(), CommentSkipper<Iterator> >
{
    TestGrammar() : TestGrammar::base_type(start)
    {
        using ascii::char_;
		start = *(char_ - ',') % ",";;
    }

	qi::rule<Iterator, StringList(), CommentSkipper<Iterator> > start;
};

std::vector<std::string> ParseString(std::string const & str)
{
	typedef std::string::const_iterator iterator_type;
	iterator_type iter = str.begin();
    iterator_type end = str.end();

	std::vector<std::string> ret;
	TestGrammar<iterator_type> grammar;
	CommentSkipper<iterator_type> skipper;
	bool success = qi::phrase_parse(iter, end, grammar, skipper, ret);
	BOOST_CHECK(iter == end); // Can't print these...

	return ret;
}

// Actual tests

BOOST_AUTO_TEST_CASE(OneLineComments)
{
	auto result = ParseString("one, two, // three, four, five\n six, seven");
	BOOST_REQUIRE_EQUAL(result.size(), 4);
	BOOST_CHECK_EQUAL(result[0], "one");
	BOOST_CHECK_EQUAL(result[1], "two");
	BOOST_CHECK_EQUAL(result[2], "six");
	BOOST_CHECK_EQUAL(result[3], "seven");
}

BOOST_AUTO_TEST_CASE(InsideLineComments)
{
	auto result = ParseString("one, two, /* three, four, five */ six, seven");
	BOOST_REQUIRE_EQUAL(result.size(), 4);
	BOOST_CHECK_EQUAL(result[0], "one");
	BOOST_CHECK_EQUAL(result[1], "two");
	BOOST_CHECK_EQUAL(result[2], "six");
	BOOST_CHECK_EQUAL(result[3], "seven");
}

BOOST_AUTO_TEST_CASE(AcrossLineComments)
{
	auto result = ParseString("one, two, /* three, \n four, five */ six, seven");
	BOOST_REQUIRE_EQUAL(result.size(), 4);
	BOOST_CHECK_EQUAL(result[0], "one");
	BOOST_CHECK_EQUAL(result[1], "two");
	BOOST_CHECK_EQUAL(result[2], "six");
	BOOST_CHECK_EQUAL(result[3], "seven");
}

BOOST_AUTO_TEST_CASE(OneLinerNested)
{
	auto result = ParseString("one, two, /* three, //four, \n five */ six, seven");
	BOOST_REQUIRE_EQUAL(result.size(), 4);
	BOOST_CHECK_EQUAL(result[0], "one");
	BOOST_CHECK_EQUAL(result[1], "two");
	BOOST_CHECK_EQUAL(result[2], "six");
	BOOST_CHECK_EQUAL(result[3], "seven");
}

BOOST_AUTO_TEST_CASE(MultiLineNested)
{
	auto result = ParseString("one, two, /* three, /* four, /* extra, foo */, bar, */ five */ six, seven");
	//BOOST_REQUIRE_EQUAL(result.size(), 4);
	BOOST_CHECK_EQUAL(result[0], "one");
	BOOST_CHECK_EQUAL(result[1], "two");
	BOOST_CHECK_EQUAL(result[2], "six");
	BOOST_CHECK_EQUAL(result[3], "seven");
}

BOOST_AUTO_TEST_SUITE_END()
