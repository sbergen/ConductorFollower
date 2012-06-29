#include <boost/test/unit_test.hpp>

#include <string>

#include "keyswitch.h"

BOOST_AUTO_TEST_SUITE(KeyswitchParserTests)

using namespace cf;
using namespace cf::Data;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

// Test grammar
template <typename Iterator>
struct TestGrammar : qi::grammar<Iterator, int(), ascii::space_type>
{
    TestGrammar() : TestGrammar::base_type(start)
    {
		start %= keyswitch;
    }

	keyswitch::grammar<Iterator, ascii::space_type> keyswitch;
	qi::rule<Iterator, int(), ascii::space_type> start;
};

int ParseString(std::string const & str)
{
	typedef std::string::const_iterator iterator_type;
	iterator_type iter = str.begin();
    iterator_type end = str.end();

	int ret = -1;
	TestGrammar<iterator_type> grammar;
	bool success = qi::phrase_parse(iter, end, grammar, ascii::space, ret);
	BOOST_CHECK(iter == end); // Can't print these...

	return ret;
}

// Actual tests

BOOST_AUTO_TEST_CASE(TestRandomNotes)
{
	int r;
	
	r = ParseString("C-5");
	BOOST_CHECK_EQUAL(r, 0);
	
	r = ParseString("G#-4");
	BOOST_CHECK_EQUAL(r, 20);

	r = ParseString("E0");
	BOOST_CHECK_EQUAL(r, 64);

	r = ParseString("B2");
	BOOST_CHECK_EQUAL(r, 95);

	r = ParseString("F#5");
	BOOST_CHECK_EQUAL(r, 126);
}

BOOST_AUTO_TEST_CASE(TestOneOctave)
{
	int r;
	
	r = ParseString("C0");
	BOOST_CHECK_EQUAL(r, 60);
	
	r = ParseString("C#0");
	BOOST_CHECK_EQUAL(r, 61);

	r = ParseString("D0");
	BOOST_CHECK_EQUAL(r, 62);

	r = ParseString("D#0");
	BOOST_CHECK_EQUAL(r, 63);

	r = ParseString("E0");
	BOOST_CHECK_EQUAL(r, 64);

	r = ParseString("F0");
	BOOST_CHECK_EQUAL(r, 65);

	r = ParseString("F#0");
	BOOST_CHECK_EQUAL(r, 66);

	r = ParseString("G0");
	BOOST_CHECK_EQUAL(r, 67);

	r = ParseString("G#0");
	BOOST_CHECK_EQUAL(r, 68);

	r = ParseString("A0");
	BOOST_CHECK_EQUAL(r, 69);

	r = ParseString("A#0");
	BOOST_CHECK_EQUAL(r, 70);

	r = ParseString("B0");
	BOOST_CHECK_EQUAL(r, 71);
}

BOOST_AUTO_TEST_SUITE_END()
