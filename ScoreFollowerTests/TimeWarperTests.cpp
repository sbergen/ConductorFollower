#include "TimeWarper.h"

BOOST_AUTO_TEST_SUITE(TimeWarperTests)

using namespace cf;
using namespace cf::ScoreFollower;

BOOST_AUTO_TEST_CASE(TestLinearWarp)
{
	real_time_t referenceRealTime = time::now();
	score_time_t referenceScoreTime = score_time_t::zero();

	speed_t linearSpeed = 2.0;

	// Fix initial speed
	TimeWarper warper;
	warper.FixTimeMapping(referenceRealTime, referenceScoreTime, linearSpeed);

	// Take a reference point and do a manual warp
	score_time_t originalScoreTime = score_time_t(100);
	score_time_t expectedScoreTime = time::multiply(originalScoreTime, linearSpeed);

	// Calculate the respective real time
	real_time_t realTime = referenceRealTime + time::duration_cast<duration_t>(originalScoreTime);

	// Check
	BOOST_CHECK_EQUAL(warper.WarpTimestamp(realTime), expectedScoreTime);
}

BOOST_AUTO_TEST_CASE(TestWarpAtSpeedChange)
{
	real_time_t referenceRealTime = time::now();
	score_time_t referenceScoreTime = score_time_t::zero();

	speed_t linearSpeed = 2.0;

	// Fix initial speed
	TimeWarper warper;
	warper.FixTimeMapping(referenceRealTime, referenceScoreTime, linearSpeed);

	// Take a reference point and do a manual warp
	score_time_t originalScoreTime = score_time_t(100);
	score_time_t expectedScoreTime = time::multiply(originalScoreTime, linearSpeed);

	// Calculate the respective real time
	real_time_t realTime = referenceRealTime + time::duration_cast<duration_t>(originalScoreTime);

	// Fix a warp point there
	warper.FixTimeMapping(realTime, expectedScoreTime, 5.0);

	// Check
	BOOST_CHECK_EQUAL(warper.WarpTimestamp(realTime), expectedScoreTime);
}

BOOST_AUTO_TEST_CASE(TestMoreComplexWarp)
{
	real_time_t referenceRealTime = time::now();
	score_time_t referenceScoreTime = score_time_t::zero();

	speed_t speed1 = 2.0;
	speed_t speed2 = 3.5;

	// Fix initial speed
	TimeWarper warper;
	warper.FixTimeMapping(referenceRealTime, referenceScoreTime, speed1);

	// Create a second warp point
	score_time_t warpPointSt = score_time_t(1000000000);
	real_time_t warpPointRt = warper.InverseWarpTimestamp(warpPointSt);
	warper.FixTimeMapping(warpPointRt, warpPointSt, speed2);

	// Warp around point
	duration_t interval = duration_t(10000);
	score_time_t beforeWarped = warper.WarpTimestamp(warpPointRt - interval);
	score_time_t afterWarped = warper.WarpTimestamp(warpPointRt + interval);

	// Manual warp
	score_time_t intervalSt = time::duration_cast<score_time_t>(interval);
	score_time_t beforeExpected = warpPointSt - time::multiply(intervalSt, speed1);
	score_time_t afterExpected = warpPointSt + time::multiply(intervalSt, speed2);

	// Check
	BOOST_CHECK_EQUAL(beforeWarped, beforeExpected);
	BOOST_CHECK_EQUAL(afterWarped, afterExpected);
}



BOOST_AUTO_TEST_SUITE_END()
