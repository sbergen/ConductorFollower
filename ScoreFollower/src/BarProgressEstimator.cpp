#include "BarProgressEstimator.h"

#include "cf/algorithm.h"

#include "BeatClassification.h"

#define DEBUG_BAR_PROGRESS_ESTIMATOR 1

namespace cf {
namespace ScoreFollower {

BarProgressEstimator::BarProgressEstimator(Data::BeatPattern const & pattern)
	: pattern_(pattern)
	, qualityForThisBar_(0.0)
{
	for (auto i = 0; i < pattern.beats.size(); ++i) {
		beats_.push_back(Beat(pattern, i));
	}
}

BeatClassification
BarProgressEstimator::ClassifyBeat(timestamp_t const & timestamp,
	ScorePosition const & position, beat_pos_t beginningOfBar, beat_pos_t offsetEstimate)
{
	ResetIfBarChanged(beginningOfBar);
	
	// Positions
	OffsetPair positions(
		position.position() - beginningOfBar,
		position.position() - beginningOfBar - offsetEstimate);

	// Best from this bar
	auto beatIt = ClassifyBeat(positions.estimation);
	auto thisBarScore = beatIt->scorer.ScoreForBeat(positions.estimation);
	AddPenaltyForUnusedBeats(beatIt);

	// Offsets
	OffsetPair offsets(
		beatIt->scorer.OffsetTo(positions.absolute),
		beatIt->scorer.OffsetTo(positions.estimation));

#if DEBUG_BAR_PROGRESS_ESTIMATOR
	LOG("Relative position: %1%, estimation position: %2%",
		positions.absolute, positions.estimation);
	LOG("Offset: %1%, estimation offset: %2%, beat already used: %3%",
		offsets.absolute, offsets.estimation, beatIt->used);
#endif

	// Check next bar
	auto nextBarOffsets = OffsetsForNextBar(positions);
	auto nextBarScore = beats_.front().scorer.ScoreForBeat(nextBarOffsets.estimation);
	if (nextBarScore > thisBarScore) {
		qualityForThisBar_ += nextBarScore;
		return BeatClassification(timestamp, position, BeatClassification::NextBar, nextBarOffsets.absolute, qualityForThisBar_);
	}

	// Else evaluate for this bar
	if(!beatIt->used) {
		beatIt->used = true;
		qualityForThisBar_ += thisBarScore;
		LOG("Progress estimator returning offset: %1%, quality: %2%", offsets.absolute, qualityForThisBar_);
		return BeatClassification(timestamp, position, BeatClassification::CurrentBar, offsets.absolute, qualityForThisBar_);
	} else {
		qualityForThisBar_ += beatIt->scorer.PenaltyForUsed();
		return BeatClassification(timestamp, position);
	}
}

void
BarProgressEstimator::ResetIfBarChanged(beat_pos_t const & beginningOfBar)
{
	if (beginningOfBar != previousBeginningOfBar_) {
		previousBeginningOfBar_ = beginningOfBar;
		for (auto it = std::begin(beats_); it != std::end(beats_); ++it) {
			it->used = false;
		}
		qualityForThisBar_ = 0.0;
	}
}

BarProgressEstimator::BeatList::iterator
BarProgressEstimator::ClassifyBeat(beat_pos_t const & estimationPosition)
{
	BeatScorer::score_t bestEstimate = std::numeric_limits<BeatScorer::score_t>::lowest();
	BeatList::iterator bestIt = std::end(beats_);

	for (auto it = std::begin(beats_); it != std::end(beats_); ++it) {
		BeatScorer::score_t estimate = it->scorer.ScoreForBeat(estimationPosition);

		if (estimate > bestEstimate) {
			bestEstimate = estimate;
			bestIt = it;
		}
	}

	return bestIt;
}

void
BarProgressEstimator::AddPenaltyForUnusedBeats(BeatList::iterator currentBeat)
{
	for (auto it = std::begin(beats_); it != currentBeat; ++it) {
		if (!it->used) {
			LOG("Adding penalty for unused!");
			qualityForThisBar_ += it->scorer.PenaltyForMissed();
			it->used = true;
		}
	}
}

BarProgressEstimator::OffsetPair
BarProgressEstimator::OffsetsForNextBar(OffsetPair const & positions)
{
	auto barLength = pattern_.meter.BarDuration() * score::bar;
	auto offset = positions.absolute - barLength;
	auto estimationOffset = positions.estimation - barLength;
	return OffsetPair(offset, estimationOffset);
}

} // namespace ScoreFollower
} // namespace cf
