#include <boost/test/unit_test.hpp>

#include "cf/score_units.h"

BOOST_AUTO_TEST_SUITE(ScoreUnitsTests)

namespace score = cf::score;
namespace bu = boost::units;

typedef bu::quantity<score::musical_time> beat_duration;
typedef bu::quantity<score::physical_time> time_duration;
typedef bu::quantity<score::tempo> tempo_t;

BOOST_AUTO_TEST_CASE(TestBasics)
{
	beat_duration beats(120 * score::beats);
	time_duration minute(60 * score::seconds);

	tempo_t tempo(beats / minute);
	BOOST_CHECK_CLOSE(tempo.value(), 2.0, 0.01);
}

BOOST_AUTO_TEST_CASE(TestConversion)
{
	beat_duration beats(120 * score::beats);
	time_duration minute(60 * boost::units::si::seconds);

	tempo_t tempo(beats / minute);
	BOOST_CHECK_CLOSE(tempo.value(), 2.0, 0.01);
}

BOOST_AUTO_TEST_CASE(TestSmallConversion)
{
	// Test that the integer conversion factor doesn't do anything funky
	time_duration dur(0.01 * boost::units::si::seconds);
	boost::units::quantity<boost::units::si::time> dur2(dur);
	BOOST_CHECK_EQUAL(dur.value(), dur2.value());
}

BOOST_AUTO_TEST_CASE(TestDivisions)
{
	beat_duration dur(2.0 * score::eight_note);
	beat_duration dur2(1.0 * score::quarter_note);
	BOOST_CHECK_CLOSE(dur.value(), dur2.value(), 0.01);

	dur = beat_duration(1.0 * score::full_note);
	dur2 = beat_duration(8.0 * score::eight_note);
	BOOST_CHECK_CLOSE(dur.value(), dur2.value(), 0.01);

	dur = beat_duration(1.0 * score::half_note);
	dur2 = beat_duration(8.0 * score::sixteenth_note);
	BOOST_CHECK_CLOSE(dur.value(), dur2.value(), 0.01);
}

BOOST_AUTO_TEST_SUITE_END()