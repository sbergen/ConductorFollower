#include <boost/test/unit_test.hpp>

#include <string>

#include "src/time_signature.h"

BOOST_AUTO_TEST_SUITE(TimeSignatureTests)

using namespace cf;
using namespace cf::Data;

namespace ascii = boost::spirit::ascii;

TimeSignature ParseString(std::string const & str)
{
	typedef std::string::const_iterator iterator_type;
	iterator_type iter = str.begin();
    iterator_type end = str.end();

	TimeSignature ret;
	time_signature::grammar<iterator_type, ascii::space_type> grammar;
	bool success = qi::phrase_parse(iter, end, grammar, ascii::space, ret);
	BOOST_CHECK(iter == end); // Can't print these...

	return ret;
}

// Actual tests

BOOST_AUTO_TEST_CASE(TestVariousMeters)
{
	TimeSignature meter;
	
	meter = ParseString("1/4");
	BOOST_CHECK_EQUAL(meter.count(), 1);
	BOOST_CHECK_EQUAL(meter.division(), 4);

	meter = ParseString("2/4");
	BOOST_CHECK_EQUAL(meter.count(), 2);
	BOOST_CHECK_EQUAL(meter.division(), 4);

	meter = ParseString("4/4");
	BOOST_CHECK_EQUAL(meter.count(), 4);
	BOOST_CHECK_EQUAL(meter.count(), 4);

	meter = ParseString("6/8");
	BOOST_CHECK_EQUAL(meter.count(), 6);
	BOOST_CHECK_EQUAL(meter.division(), 8);

	meter = ParseString("2/2");
	BOOST_CHECK_EQUAL(meter.count(), 2);
	BOOST_CHECK_EQUAL(meter.division(), 2);
}

BOOST_AUTO_TEST_SUITE_END()
