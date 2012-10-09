#include <boost/test/unit_test.hpp>

#include <boost/units/io.hpp>

#include "cf/globals.h"
#include "src/TempoFunction.h"

BOOST_AUTO_TEST_SUITE(TempoFunctionTests)

using namespace cf;
using namespace cf::ScoreFollower;

real_time_t make_time(real_time_t ref, time_quantity offset)
{
	return ref + time_cast<real_time_t::duration>(offset);
}

void RunEndTestWith(tempo_t tempoChange, beats_t catchup)
{
	TempoFunction f;
	real_time_t refTime = time::now();
	auto time = 1.0 * score::seconds;
	auto refTempo = 1.0 * score::beats_per_second;

	f.SetParameters(
		refTime, time, 
		refTempo, tempoChange,
		catchup, 1.0);

	real_time_t checkTime = make_time(refTime, time);
	BOOST_CHECK_CLOSE(f.TempoAt(checkTime).value(), (refTempo + tempoChange).value(), 0.001);
	BOOST_CHECK_CLOSE(f.OffsetAt(checkTime).value(), 0.0, 0.001);

	// Should stay the same
	checkTime = make_time(refTime, 2.0 * time);
	BOOST_CHECK_CLOSE(f.TempoAt(checkTime).value(), (refTempo + tempoChange).value(), 0.001);
	BOOST_CHECK_CLOSE(f.OffsetAt(checkTime).value(), 0.0, 0.001);
}

void RunIntegralTestWith(tempo_t tempoChange, beats_t catchup)
{
	TempoFunction f;
	real_time_t refTime = time::now();
	auto time = 1.5 * score::seconds;
	auto refTempo = 2.0 * score::beats_per_second;

	int steps = 100;
	auto step = time / (double)steps;

	f.SetParameters(
		refTime, time, 
		refTempo, tempoChange,
		catchup, 1.0);

	beat_pos_t position = 0.0 * score::beats;
	for (int i = 0; i < steps; ++i) {
		auto timeNow = make_time(refTime, (double)i * step);
		auto tempo = f.TempoAt(timeNow);
		position += tempo * step;
	}

	auto linearPosition = (refTempo + tempoChange) * time;
	BOOST_CHECK_CLOSE((linearPosition + catchup).value(), position.value(), 1.0);
}

void RunTestWith(tempo_t tempoChange, beats_t catchup)
{
	RunEndTestWith(tempoChange, catchup);
	RunIntegralTestWith(tempoChange, catchup);
}

BOOST_AUTO_TEST_CASE(TestConstantTempo)
{
	TempoFunction f;
	f.SetConstantTempo(1.0 * score::beats_per_second);
	BOOST_CHECK_EQUAL(f.TempoAt(time::now()), 1.0 * score::beats_per_second);
}

BOOST_AUTO_TEST_CASE(TestNoChange)
{
	RunTestWith(0.0 * score::beats_per_second, 0.0 * score::beats);
}

BOOST_AUTO_TEST_CASE(TestNoTempoChangeCatchup)
{
	RunTestWith(0.0 * score::beats_per_second, 1.0 * score::beats);
	RunTestWith(0.0 * score::beats_per_second, -0.5 * score::beats);
}

BOOST_AUTO_TEST_CASE(TestNoCatchupTempoChange)
{
	RunTestWith(1.1 * score::beats_per_second, 0.0 * score::beats);
	RunTestWith(-0.7 * score::beats_per_second, 0.0 * score::beats);
}

BOOST_AUTO_TEST_CASE(TestCatchupAndTempoChange)
{
	RunTestWith(1.2 * score::beats_per_second, 1.1 * score::beats);
	RunTestWith(-0.3 * score::beats_per_second, -0.1 * score::beats);
	RunTestWith(-0.2 * score::beats_per_second, 2.1 * score::beats);
	RunTestWith(0.7 * score::beats_per_second, -1.6 * score::beats);
}

BOOST_AUTO_TEST_SUITE_END()
