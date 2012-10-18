#include <boost/test/unit_test.hpp>

#include "src/BarProgressEstimator.h"
#include "src/BeatClassification.h"

#include "BeatPatternCommon.h"

BOOST_AUTO_TEST_SUITE(BarProgressEstimatorTests)

using namespace cf;
using namespace cf::ScoreFollower;

BOOST_AUTO_TEST_CASE(TestSimpleSelection)
{
	BarProgressEstimator qEstimator(make_quarter_pattern());
	BarProgressEstimator hEstimator(make_half_pattern());

	ScorePosition pos; // Default constructed is ok

	// First beats equal
	auto qEstimate = qEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	auto hEstimate = hEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_CLOSE(qEstimate.quality(), hEstimate.quality(), 0.01);
	
	// The rest should match quarter better
	pos = pos.ScorePositionAt(0.0 * score::bars, 1.0 * score::beats);
	qEstimate = qEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	hEstimate = hEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_GT(qEstimate.quality(), hEstimate.quality());

	pos = pos.ScorePositionAt(0.0 * score::bars, 2.0 * score::beats);
	qEstimate = qEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	hEstimate = hEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_GT(qEstimate.quality(), hEstimate.quality());

	pos = pos.ScorePositionAt(0.0 * score::bars, 3.0 * score::beats);
	qEstimate = qEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	hEstimate = hEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_GT(qEstimate.quality(), hEstimate.quality());
}

BOOST_AUTO_TEST_CASE(TestMissingOne)
{
	BarProgressEstimator qEstimator(make_quarter_pattern());
	BarProgressEstimator hEstimator(make_half_pattern());

	ScorePosition pos; // Default constructed is ok

	// First beats equal
	auto qEstimate = qEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	auto hEstimate = hEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_CLOSE(qEstimate.quality(), hEstimate.quality(), 0.01);
	
	// Second should match half
	pos = pos.ScorePositionAt(0.0 * score::bars, 2.0 * score::beats);
	qEstimate = qEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	hEstimate = hEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_GT(hEstimate.quality(), qEstimate.quality());

	// Third should match quarter
	pos = pos.ScorePositionAt(0.0 * score::bars, 3.0 * score::beats);
	qEstimate = qEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	hEstimate = hEstimator.ClassifyBeat(time::now(), pos, 0.0 * score::beats, 0.0 * score::beats);
	BOOST_CHECK_GT(qEstimate.quality(), hEstimate.quality());
}

BOOST_AUTO_TEST_SUITE_END()
