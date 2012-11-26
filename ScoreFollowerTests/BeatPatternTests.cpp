#include <boost/test/unit_test.hpp>

#include "ScoreFollower/Follower.h"

#include "src/BeatPattern.h"
#include "src/BeatClassification.h"

#include "BeatPatternCommon.h"

BOOST_AUTO_TEST_SUITE(BeatPatternTests)

using namespace cf;
using namespace cf::ScoreFollower;

BOOST_AUTO_TEST_CASE(TestOneBarOfCorrectPattern)
{
	Follower::OptionsBuffer options;
	auto pattern = BeatPattern(make_quarter_pattern(), options.GetReader());

	BeatPattern::beat_array beats;
	beats.push_back(0.0 * score::beats);
	beats.push_back(1.0 * score::beats);
	beats.push_back(2.0 * score::beats);
	beats.push_back(3.0 * score::beats);

	auto quality = pattern.MatchQuality(beats, 1.0);
	BOOST_CHECK_CLOSE(quality, 0.0, 0.01);
}

BOOST_AUTO_TEST_CASE(TestTwoCorrectBars)
{
	Follower::OptionsBuffer options;
	auto pattern = BeatPattern(make_quarter_pattern(), options.GetReader());

	BeatPattern::beat_array beats;
	beats.push_back(0.0 * score::beats);
	beats.push_back(1.0 * score::beats);
	beats.push_back(2.0 * score::beats);
	beats.push_back(3.0 * score::beats);
	beats.push_back(4.0 * score::beats);
	beats.push_back(5.0 * score::beats);
	beats.push_back(6.0 * score::beats);
	beats.push_back(7.0 * score::beats);

	auto quality = pattern.MatchQuality(beats, 1.0);
	BOOST_CHECK_CLOSE(quality, 0.0, 0.01);
}

BOOST_AUTO_TEST_CASE(TestStartAndEndIngoring)
{
	Follower::OptionsBuffer options;
	auto pattern = BeatPattern(make_quarter_pattern(), options.GetReader());

	BeatPattern::beat_array beats;
	beats.push_back(1.0 * score::beats);
	beats.push_back(2.0 * score::beats);

	auto quality = pattern.MatchQuality(beats, 1.0);
	BOOST_CHECK_CLOSE(quality, 0.0, 0.01);
}

BOOST_AUTO_TEST_CASE(TestOneMissing)
{
	Follower::OptionsBuffer options;
	auto pattern = BeatPattern(make_quarter_pattern(), options.GetReader());

	for (int i = 1; i < 3; ++i) {
		BeatPattern::beat_array beats;

		beats.push_back(0.0 * score::beats);
		if (i != 1) { beats.push_back(1.0 * score::beats); }
		if (i != 2) { beats.push_back(2.0 * score::beats); }
		beats.push_back(3.0 * score::beats);

		auto scorer = BeatScorer(make_quarter_pattern(), i);
		auto missQuality = scorer.PenaltyForMissed();
		auto quality = pattern.MatchQuality(beats, 1.0);
		BOOST_CHECK_CLOSE(quality, missQuality, 0.01);
	}
}

BOOST_AUTO_TEST_CASE(TestMissingLastInBar)
{
	Follower::OptionsBuffer options;
	auto pattern = BeatPattern(make_quarter_pattern(), options.GetReader());

	BeatPattern::beat_array beats;
	beats.push_back(0.0 * score::beats);
	beats.push_back(1.0 * score::beats);
	beats.push_back(2.0 * score::beats);
	beats.push_back(4.0 * score::beats);
	beats.push_back(5.0 * score::beats);
	beats.push_back(6.0 * score::beats);
	beats.push_back(7.0 * score::beats);

	auto scorer = BeatScorer(make_quarter_pattern(), 3);
	auto missQuality = scorer.PenaltyForMissed();
	auto quality = pattern.MatchQuality(beats, 1.0);
	BOOST_CHECK_CLOSE(quality, missQuality, 0.01);
}

BOOST_AUTO_TEST_CASE(TestMissingFirstInBar)
{
	Follower::OptionsBuffer options;
	auto pattern = BeatPattern(make_quarter_pattern(), options.GetReader());

	BeatPattern::beat_array beats;
	beats.push_back(0.0 * score::beats);
	beats.push_back(1.0 * score::beats);
	beats.push_back(2.0 * score::beats);
	beats.push_back(3.0 * score::beats);
	beats.push_back(5.0 * score::beats);
	beats.push_back(6.0 * score::beats);
	beats.push_back(7.0 * score::beats);

	auto scorer = BeatScorer(make_quarter_pattern(), 0);
	auto missQuality = scorer.PenaltyForMissed();
	auto quality = pattern.MatchQuality(beats, 1.0);
	BOOST_CHECK_CLOSE(quality, missQuality, 0.01);
}

BOOST_AUTO_TEST_CASE(TestStretchFromZero)
{
	Follower::OptionsBuffer options;
	auto pattern = BeatPattern(make_quarter_pattern(), options.GetReader());

	BeatPattern::beat_array beats;
	beats.push_back(0.0 * score::beats);
	beats.push_back(0.5 * score::beats);
	beats.push_back(1.0 * score::beats);
	beats.push_back(1.5 * score::beats);
	beats.push_back(2.0 * score::beats);
	beats.push_back(2.5 * score::beats);
	beats.push_back(3.0 * score::beats);

	auto quality = pattern.MatchQuality(beats, 2.0);
	BOOST_CHECK_CLOSE(quality, 0.0, 0.01);
}

BOOST_AUTO_TEST_CASE(TestStretchFromNonZero)
{
	Follower::OptionsBuffer options;
	auto pattern = BeatPattern(make_quarter_pattern(), options.GetReader());

	BeatPattern::beat_array beats;
	beats.push_back(1.0 * score::beats);
	beats.push_back(1.5 * score::beats);
	beats.push_back(2.0 * score::beats);
	beats.push_back(2.5 * score::beats);
	beats.push_back(3.0 * score::beats);

	auto quality = pattern.MatchQuality(beats, 2.0);
	BOOST_CHECK_CLOSE(quality, 0.0, 0.01);
}

BOOST_AUTO_TEST_SUITE_END()
