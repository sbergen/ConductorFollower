#include "BeatPattern.h"

namespace cf {
namespace ScoreFollower {

BeatPattern::BeatPattern(Data::BeatPattern const & pattern)
	: meter_(pattern.meter)
{
	for (int i = 0; i < pattern.beats.size(); ++i) {
		scorers_.push_back(BeatScorer(pattern, i));
	}
}

double
BeatPattern::MatchQuality(beat_array const & beats, double scale) const
{
	if (scorers_.size() == 0) { return 0.0; }
	auto pivot = beats.front();

	beat_array scaled;
	std::for_each(std::begin(beats), std::end(beats),
		[&](beat_pos_t const & beat)
		{
			scaled.push_back(ScaleWithPivot(beat, pivot, scale));
		});

	return MatchScaled(scaled);
}

beat_pos_t
BeatPattern::ScaleWithPivot(beat_pos_t pos, beat_pos_t pivot, double scale) const
{
	auto distanceFromPivot = pos - pivot;
	return pivot + scale * distanceFromPivot;
}

double
BeatPattern::MatchScaled(beat_array const & beats) const
{
	beat_pos_t barOffset = 0.0 * score::beats;
	auto scorerIt = scorers_.begin();
	auto score = 0.0;
	bool first = true;

	std::for_each(std::begin(beats), std::end(beats),
		[&, this](beat_pos_t const & beat)
		{
			AdvanceScorer(scorerIt, beat, barOffset, score, first);
			first = false;
		});

	return score;
}

void
BeatPattern::AdvanceScorer(scorer_array::const_iterator & it,
	beat_pos_t const & pos, beat_pos_t & barOffset,
	double & score, bool first) const
{
	std::cout << "-- Scoring: " << pos << std::endl;

	auto initial = it;

	while (true) {
		if (it == scorers_.end()) {
			std::cout << "!! Going to next bar" << std::endl;
			it = scorers_.begin();
			barOffset += meter_.BarDuration() * score::bar;
		}
		auto offsetPos = pos - barOffset;

		auto next = it;
		std::advance(next, 1);
		auto nextOffsetPos = pos - barOffset;
		if (next == scorers_.end()) {
			std::cout << "! Next is in next bar" << std::endl;
			nextOffsetPos -= meter_.BarDuration() * score::bar;
			next = scorers_.begin();
		}

		auto curScore = it->ScoreForBeat(offsetPos);
		auto nextScore = next->ScoreForBeat(nextOffsetPos);

		std::cout << "NextScore: " << nextScore << ", curScore: " << curScore << std::endl;
		if (nextScore < curScore) {
			if (it == initial && !first) {
				// duplicate classification
				score += it->BeatPenaltyForUsed();
				std::cout << "used penalty" << std::endl;
			}

			// we found the best one
			std::cout << "beat score: " << curScore << std::endl << std::endl;
			score += curScore;
			return;
		}

		if (it != initial && !first) {
			// We have skipped one
			score += it->BarPenaltyForMissed();
			std::cout << "skip penalty" << std::endl;
		}

		++it;
		std::cout << "going for next loop" << std::endl;
	}
}

} // namespace ScoreFollower
} // namespace cf
