#include <boost/test/unit_test.hpp>

#include <string>

#include "score_event.h"

BOOST_AUTO_TEST_SUITE(ScoreEventParserTests)

using namespace cf;
using namespace cf::Data;

namespace ascii = boost::spirit::ascii;

ScoreEvent ParseString(std::string const & str)
{
	typedef std::string::const_iterator iterator_type;
	iterator_type iter = str.begin();
    iterator_type end = str.end();

	ScoreEvent ret;
	score_event::grammar<iterator_type, ascii::space_type> grammar;
	bool success = qi::phrase_parse(iter, end, grammar, ascii::space, ret);
	BOOST_CHECK(iter == end); // Can't print these...

	return ret;
}

// Actual tests

BOOST_AUTO_TEST_CASE(TestTempoSensitivity)
{
	ScoreEvent e = ParseString("tempo_sensitivity { position: 5|0, sensitivity: 0.7 }");
	
	BOOST_CHECK(e.type() == typeid(TempoSensitivityChange));

	auto change = boost::get<TempoSensitivityChange>(e);
	BOOST_CHECK_EQUAL(change.position.bar, 5);
	BOOST_CHECK_CLOSE(change.position.beat, 0.0, 0.001);
	BOOST_CHECK_CLOSE(change.sensitivity, 0.7, 0.001);
}

BOOST_AUTO_TEST_CASE(TestFermata)
{
	ScoreEvent e = ParseString("fermata { tempo_reference: 4|0, position: 5|0, length: 0.5 }");
	
	BOOST_CHECK(e.type() == typeid(Fermata));

	auto fermata = boost::get<Fermata>(e);
	BOOST_CHECK_EQUAL(fermata.tempoReference.bar, 4);
	BOOST_CHECK_CLOSE(fermata.tempoReference.beat, 0.0, 0.001);
	BOOST_CHECK_EQUAL(fermata.position.bar, 5);
	BOOST_CHECK_CLOSE(fermata.position.beat, 0.0, 0.001);
	BOOST_CHECK_CLOSE(fermata.length, 0.5, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()
