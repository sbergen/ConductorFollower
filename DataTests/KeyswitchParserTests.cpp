#include <boost/test/unit_test.hpp>

#include <string>

#include "src/keyswitch.h"

BOOST_AUTO_TEST_SUITE(KeyswitchParserTests)

using namespace cf;
using namespace cf::Data;

namespace ascii = boost::spirit::ascii;

int ParseString(std::string const & str)
{
	typedef std::string::const_iterator iterator_type;
	iterator_type iter = str.begin();
    iterator_type end = str.end();

	int ret = -1;
	keyswitch::grammar<iterator_type, ascii::space_type> grammar;
	bool success = qi::phrase_parse(iter, end, grammar, ascii::space, ret);
	BOOST_CHECK(iter == end); // Can't print these...

	return ret;
}

// Actual tests

BOOST_AUTO_TEST_CASE(TestRandomNotes)
{
	int r;
	
	r = ParseString("C-1");
	BOOST_CHECK_EQUAL(r, 0);
	
	r = ParseString("G#0");
	BOOST_CHECK_EQUAL(r, 20);

	r = ParseString("E4");
	BOOST_CHECK_EQUAL(r, 64);

	r = ParseString("B6");
	BOOST_CHECK_EQUAL(r, 95);

	r = ParseString("F#9");
	BOOST_CHECK_EQUAL(r, 126);
}

BOOST_AUTO_TEST_CASE(TestOneOctave)
{
	int r;
	
	r = ParseString("C0");
	BOOST_CHECK_EQUAL(r, 12);
	
	r = ParseString("C#0");
	BOOST_CHECK_EQUAL(r, 13);

	r = ParseString("D0");
	BOOST_CHECK_EQUAL(r, 14);

	r = ParseString("D#0");
	BOOST_CHECK_EQUAL(r, 15);

	r = ParseString("E0");
	BOOST_CHECK_EQUAL(r, 16);

	r = ParseString("F0");
	BOOST_CHECK_EQUAL(r, 17);

	r = ParseString("F#0");
	BOOST_CHECK_EQUAL(r, 18);

	r = ParseString("G0");
	BOOST_CHECK_EQUAL(r, 19);

	r = ParseString("G#0");
	BOOST_CHECK_EQUAL(r, 20);

	r = ParseString("A0");
	BOOST_CHECK_EQUAL(r, 21);

	r = ParseString("A#0");
	BOOST_CHECK_EQUAL(r, 22);

	r = ParseString("B0");
	BOOST_CHECK_EQUAL(r, 23);
}

BOOST_AUTO_TEST_SUITE_END()
