#include <boost/test/unit_test.hpp>

#include <fstream>
#include <sstream>

#include "Data/InstrumentParser.h"

BOOST_AUTO_TEST_SUITE(InstrumentParserTests)

using namespace cf;
using namespace cf::Data;

BOOST_AUTO_TEST_CASE(BasicTest)
{
	std::string filename = "instruments_basic.testdata";
	
	std::ostringstream oss;
	oss
		<< "["
			<< "instrument { "
				<< "name: \"piano\","
				<< "patches: ["
					<< "patch {"
						<< "name: \"regular\""
						<< "keyswitch: 42"
						<< "t_ads: [ 0.55, 0.22, 0.11 ]"
						<< "l_ads: [ 0.55, 0.22, 0.11 ]"
					<< "},"
					<< "patch { name: \"dummy\" }" // dummy patch
				<< "]"
			<< "},"
			<< "instrument { name: \"dummy\" }" // dummy instrument
		<< "]";

	//std::cout << oss.str();

	{
		std::ofstream ofs(filename, std::ios_base::trunc);
		ofs << oss.str();
	}

	InstrumentParser parser;
	BOOST_CHECK(parser.parse(filename));
	auto instruments = parser.Instruments();

	// Instrument
	BOOST_REQUIRE_GT(instruments.size(), 0);
	BOOST_CHECK_EQUAL(instruments.size(), 2);
	auto instrument = instruments[0];
	BOOST_CHECK_EQUAL(instrument.name, "piano");

	// Patch
	BOOST_REQUIRE_GT(instrument.patches.size(), 0);
	BOOST_CHECK_EQUAL(instrument.patches.size(), 2);
	auto patch = instrument.patches[0];

	BOOST_CHECK_EQUAL(patch.name, "regular");
	BOOST_CHECK_EQUAL(patch.keyswitch, 42);

	BOOST_CHECK_EQUAL(patch.envelopeTimes.attack, 0.55);
	BOOST_CHECK_EQUAL(patch.envelopeTimes.decay, 0.22);
	BOOST_CHECK_EQUAL(patch.envelopeTimes.sustain, 0.11);

	BOOST_CHECK_EQUAL(patch.envelopeLevels.attack, 0.55);
	BOOST_CHECK_EQUAL(patch.envelopeLevels.decay, 0.22);
	BOOST_CHECK_EQUAL(patch.envelopeLevels.sustain, 0.11);
}

BOOST_AUTO_TEST_SUITE_END()
