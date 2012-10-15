#include <boost/test/unit_test.hpp>

#include "src/BarProgressEstimator.h"
#include "src/BeatClassification.h"

BOOST_AUTO_TEST_SUITE(BarProgressEstimatorTests)

using namespace cf;
using namespace cf::ScoreFollower;

real_time_t make_time(real_time_t ref, time_quantity offset)
{
	return ref + time_cast<real_time_t::duration>(offset);
}

BarProgressEstimator make_quarter_estimator()
{
	Data::BeatPattern pattern;
	pattern.meter = TimeSignature(4, 4);
	
	Data::Beat beat;

	beat.time = 0.0;
	pattern.beats.push_back(beat);

	beat.time = 1.0;
	pattern.beats.push_back(beat);

	beat.time = 2.0;
	pattern.beats.push_back(beat);

	beat.time = 3.0;
	pattern.beats.push_back(beat);

	return pattern;
}

BarProgressEstimator make_half_estimator()
{
	Data::BeatPattern pattern;
	pattern.meter = TimeSignature(4, 4);
	
	Data::Beat beat;

	beat.time = 0.0;
	pattern.beats.push_back(beat);

	beat.time = 2.0;
	pattern.beats.push_back(beat);

	return pattern;
}

BOOST_AUTO_TEST_CASE(TestSimpleSelection)
{
	auto qEstimator = make_quarter_estimator();
	auto hEstimator = make_half_estimator();

	ScorePosition pos; // Default constructed is ok

	// First beats equal
	auto qEstimate = qEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	auto hEstimate = hEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_CLOSE(qEstimate.quality(), hEstimate.quality(), 0.01);
	
	// The rest should match quarter better
	pos = pos.ScorePositionAt(0.0 * score::bars, 1.0 * score::beats);
	qEstimate = qEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	hEstimate = hEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_GT(qEstimate.quality(), hEstimate.quality());

	pos = pos.ScorePositionAt(0.0 * score::bars, 2.0 * score::beats);
	qEstimate = qEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	hEstimate = hEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_GT(qEstimate.quality(), hEstimate.quality());

	pos = pos.ScorePositionAt(0.0 * score::bars, 3.0 * score::beats);
	qEstimate = qEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	hEstimate = hEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_GT(qEstimate.quality(), hEstimate.quality());
}

BOOST_AUTO_TEST_CASE(TestMissingOne)
{
	auto qEstimator = make_quarter_estimator();
	auto hEstimator = make_half_estimator();

	ScorePosition pos; // Default constructed is ok

	// First beats equal
	auto qEstimate = qEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	auto hEstimate = hEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_CLOSE(qEstimate.quality(), hEstimate.quality(), 0.01);
	
	// Second should match half
	pos = pos.ScorePositionAt(0.0 * score::bars, 2.0 * score::beats);
	qEstimate = qEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	hEstimate = hEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_GT(hEstimate.quality(), qEstimate.quality());

	// Third should match quarter
	pos = pos.ScorePositionAt(0.0 * score::bars, 3.0 * score::beats);
	qEstimate = qEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	hEstimate = hEstimator.ClassifyBeat(pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_GT(qEstimate.quality(), hEstimate.quality());
}

BOOST_AUTO_TEST_SUITE_END()
