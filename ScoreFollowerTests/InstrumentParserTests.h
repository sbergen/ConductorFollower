#include "InstrumentParser.h"

BOOST_AUTO_TEST_SUITE(InstrumentParserTests)

using namespace cf;
using namespace cf::ScoreFollower;

BOOST_AUTO_TEST_CASE(BasicTest)
{
	typedef std::string::const_iterator iterator_type;
    typedef InstrumentParser<iterator_type> Parser;

	std::string data = "[ instrument { name: \"piano\", channel: 1, programChanges: [1, 2] }, instrument { name: \"violin\", channel: 2, programChanges: [3, 4] } ]";
	Parser p;
	
	std::vector<InstrumentDefinition> defs;
	iterator_type iter = data.begin();
    iterator_type end = data.end();
	bool success = qi::phrase_parse(iter, end, p, ascii::space, defs);
	
	BOOST_REQUIRE(success);
	BOOST_REQUIRE(iter == end);
	BOOST_REQUIRE(defs.size() > 0);

	InstrumentDefinition def = defs[0];
	BOOST_CHECK_EQUAL(def.name, "piano");
	BOOST_CHECK_EQUAL(def.outChannel, 1);
	BOOST_CHECK_EQUAL(def.programChanges[0], 1);
	BOOST_CHECK_EQUAL(def.programChanges[1], 2);

}

BOOST_AUTO_TEST_SUITE_END()
