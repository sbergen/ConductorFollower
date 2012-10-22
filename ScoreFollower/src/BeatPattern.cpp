#include "BeatPattern.h"

#include "cf/algorithm.h"
#include "cf/globals.h"

#include "BeatClassification.h"

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
	IterationHelper helper(*this);
	std::for_each(std::begin(beats), std::end(beats),
		[&, this](beat_pos_t const & beat)
		{
			helper.Advance(beat);
		});

	return helper.score;
}

beat_pos_t
BeatPattern::OffsetToBest(beat_pos_t const & pos) const
{
	IterationHelper helper(*this);
	helper.Advance(pos);
	return helper.it->OffsetTo(pos - helper.barOffset);
}

bool
BeatPattern::IsConfidentEstimate(BeatClassification const & classification) const
{
	auto pos = classification.position().beat();
	IterationHelper helper(*this);
	helper.Advance(pos);

	auto best = helper.it;
	auto next = best, prev = best;
	WrappingAdvance(prev, -1);
	WrappingAdvance(next, 1);

	// Assumes all scores are negative or zero!
	double factor = 0.3;
	return (best->ScoreForBeat(pos) > factor * next->ScoreForBeat(pos)) &&
	       (best->ScoreForBeat(pos) > factor * prev->ScoreForBeat(pos));
}

void
BeatPattern::IterationHelper::Advance(beat_pos_t const & pos)
{
	//std::cout << "-- Scoring: " << pos << std::endl;

	auto initial = it;

	while (true) {
		if (it == parent.scorers_.end()) {
			//std::cout << "!! Going to next bar" << std::endl;
			it = parent.scorers_.begin();
			barOffset += parent.meter_.BarDuration() * score::bar;
		}
		auto offsetPos = pos - barOffset;

		auto next = it;
		std::advance(next, 1);
		auto nextOffsetPos = pos - barOffset;
		if (next == parent.scorers_.end()) {
			//std::cout << "! Next is in next bar" << std::endl;
			nextOffsetPos -= parent.meter_.BarDuration() * score::bar;
			next = parent.scorers_.begin();
		}

		auto curScore = it->ScoreForBeat(offsetPos);
		auto nextScore = next->ScoreForBeat(nextOffsetPos);

		//std::cout << "NextScore: " << nextScore << ", curScore: " << curScore << std::endl;
		if (nextScore < curScore) {
			if (it == initial && !first) {
				// duplicate classification
				score += it->BeatPenaltyForUsed();
				//std::cout << "used penalty" << std::endl;
			}

			// we found the best one
			//std::cout << "beat score: " << curScore << std::endl << std::endl;
			score += curScore;
			break;
		}

		if (it != initial && !first) {
			// We have skipped one
			score += it->BarPenaltyForMissed();
			//std::cout << "skip penalty" << std::endl;
		}

		++it;
		//std::cout << "going for next loop" << std::endl;
	}

	first = false;
}

} // namespace ScoreFollower
} // namespace cf
