#include <boost/test/unit_test.hpp>

#include <string>

#include "score_position.h"

BOOST_AUTO_TEST_SUITE(KeyswitchParserTests)

using namespace cf;
using namespace cf::Data;

namespace ascii = boost::spirit::ascii;

ScorePosition ParseString(std::string const & str)
{
	typedef std::string::const_iterator iterator_type;
	iterator_type iter = str.begin();
    iterator_type end = str.end();

	ScorePosition ret;
	score_position::grammar<iterator_type, ascii::space_type> grammar;
	bool success = qi::phrase_parse(iter, end, grammar, ascii::space, ret);
	BOOST_CHECK(iter == end); // Can't print these...

	return ret;
}

// Actual tests

BOOST_AUTO_TEST_CASE(TestPositions)
{
	ScorePosition pos;
	
	pos = ParseString("1|1.5");
	BOOST_CHECK_EQUAL(pos.bar, 1);
	BOOST_CHECK_CLOSE(pos.beat, 1.5, 0.001);

	pos = ParseString("-1|1.5");
	BOOST_CHECK_EQUAL(pos.bar, -1);
	BOOST_CHECK_CLOSE(pos.beat, 1.5, 0.001);

	pos = ParseString("0|2.5");
	BOOST_CHECK_EQUAL(pos.bar, 0);
	BOOST_CHECK_CLOSE(pos.beat, 2.5, 0.001);

	pos = ParseString("42|1.234");
	BOOST_CHECK_EQUAL(pos.bar, 42);
	BOOST_CHECK_CLOSE(pos.beat, 1.234, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()
