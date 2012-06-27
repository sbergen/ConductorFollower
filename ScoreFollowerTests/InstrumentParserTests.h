#include "InstrumentParser.h"

#include <fstream>

BOOST_AUTO_TEST_SUITE(InstrumentParserTests)

using namespace cf;
using namespace cf::ScoreFollower;

BOOST_AUTO_TEST_CASE(BasicTest)
{
	std::string filename = "test.testdata";

	{
		std::ofstream ofs(filename, std::ios_base::trunc);
		ofs << "["
			<< "instrument { name: \"piano\", channel: 1, programChanges: [1, 2] },"
			<< "instrument { name: \"violin\", channel: 2, programChanges: [3, 4] }"
			<< "]";
	}

	InstrumentParser parser(filename);
	
	InstrumentDefinitionList const & defs = parser.Instruments();

	BOOST_REQUIRE(defs.size() > 0);

	InstrumentDefinition def = defs[0];
	BOOST_CHECK_EQUAL(def.name, "piano");
	BOOST_CHECK_EQUAL(def.outChannel, 1);
	BOOST_CHECK_EQUAL(def.programChanges[0], 1);
	BOOST_CHECK_EQUAL(def.programChanges[1], 2);

}

BOOST_AUTO_TEST_SUITE_END()
