#include <boost/test/unit_test.hpp>

#include <fstream>
#include <sstream>

#include "Data/ScoreParser.h"

BOOST_AUTO_TEST_SUITE(ScoreParserTests)

using namespace cf;
using namespace cf::Data;

BOOST_AUTO_TEST_CASE(BasicTest)
{
	std::string filename = "score_basic.testdata";
	
	std::ostringstream oss;
	oss
		<< "score { "
			<< "name: \"Never gonna give you up\","
			<< "midi_file: \"/home/user/nggyu.mid\","
			<< "instrument_file: \"/home/user/instr.def\","
			<< "tracks: ["
				<< "track {"
					<< "name: \"piano\","
					<< "instrument: \"piano\""
				<< "},"
				<< "track { }" // dummy track
			<< "],"
			<< "events: ["
				<< "tempo_sensitivity { position: 0|0, sensitivity: 0.8 }"
				<< "tempo_sensitivity { position: 1|2, sensitivity: 0.5 }"
			<< "]"
		<< "}";

	//std::cout << oss.str();

	{
		std::ofstream ofs(filename, std::ios_base::trunc);
		ofs << oss.str();
	}

	ScoreParser parser;
	BOOST_CHECK_NO_THROW(parser.parse(filename));
	auto score = parser.data();

	// Score
	BOOST_CHECK_EQUAL(score.name, "Never gonna give you up");
	BOOST_CHECK_EQUAL(score.midiFile, "/home/user/nggyu.mid");
	BOOST_CHECK_EQUAL(score.instrumentFile, "/home/user/instr.def");
	
	// Tracks
	BOOST_REQUIRE_GT(score.tracks.size(), 0);
	BOOST_CHECK_EQUAL(score.tracks.size(), 2);
	auto track = score.tracks[0];
	BOOST_CHECK_EQUAL(track.name, "piano");
	BOOST_CHECK_EQUAL(track.instrument, "piano");

	// Events
	BOOST_REQUIRE_GT(score.events.size(), 0);
	BOOST_CHECK_EQUAL(score.events.size(), 2);
	BOOST_REQUIRE(score.events[0].type() == typeid(TempoSensitivityChange));
	auto tempoSensitivityChange = boost::get<TempoSensitivityChange>(score.events[0]);
	BOOST_CHECK_EQUAL(tempoSensitivityChange.position.bar, 0);
	BOOST_CHECK_EQUAL(tempoSensitivityChange.position.beat, 0.0);
	BOOST_CHECK_CLOSE(tempoSensitivityChange.sensitivity, 0.8, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()
