#include "BarProgressEstimator.h"

#include "cf/algorithm.h"

#include "BeatClassification.h"

namespace cf {
namespace ScoreFollower {

BarProgressEstimator::BarProgressEstimator(Data::BeatPattern const & pattern)
	: pattern_(pattern)
	, qualityForThisBar_(0.0)
{
	for (auto it = std::begin(pattern_.beats); it != std::end(pattern_.beats); ++it) {
		beats_.push_back(Beat(it->time * score::beats));
	}
}

BeatClassification
BarProgressEstimator::ClassifyBeat(ScorePosition const & position, beat_pos_t beginningOfBar, beat_pos_t offsetEstimate)
{
	ResetIfBarChanged(beginningOfBar);
	
	// Positions
	OffsetPair positions(
		position.position() - beginningOfBar,
		position.position() - beginningOfBar - offsetEstimate);
	LOG("Relative position: %1%, estimation position: %2%",
		positions.absolute, positions.estimation);

	// nearest neighbour
	auto beatIt = ClassifyBeat(positions.estimation);
	AddPenaltyForUnusedBeats(beatIt);

	// Offsets
	OffsetPair offsets(
		positions.absolute - beatIt->position,
		positions.estimation - beatIt->position);
	LOG("Offset: %1%, estimation offset: %2%, beat already used: %3%",
		offsets.absolute, offsets.estimation, beatIt->used);

	auto nextBarOffsets = EstimateForNextBar(positions);
	// Check end of bar
	if (boost::units::abs(nextBarOffsets.estimation) < boost::units::abs(offsets.estimation)) {
		qualityForThisBar_ += BeatClassification::QualityFromOffset(nextBarOffsets.estimation);
		return BeatClassification(position, BeatClassification::NextBar, nextBarOffsets.absolute, qualityForThisBar_);
	}

	// Else evaluate for this bar
	qualityForThisBar_ += BeatClassification::QualityFromOffset(offsets.estimation);
	if(!beatIt->used) {
		beatIt->used = true;
		return BeatClassification(position, BeatClassification::CurrentBar, offsets.absolute, qualityForThisBar_);
	} else {
		return BeatClassification(position);
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
	beat_pos_t bestEstimatedOffset = 100.0 * score::beats;
	BeatList::iterator bestIt = std::end(beats_);

	for (auto it = std::begin(beats_); it != std::end(beats_); ++it) {
		// TODO adjust penalty amount?
		// This penalty is only for selection, will not apply to bar quality
		auto penaltyBarFraction = (0.25 / beats_.size()) * score::bars;
		auto penaltyOffset = pattern_.meter.BarDuration() * penaltyBarFraction;
		beat_pos_t estimatedOffset =
			boost::units::abs(estimationPosition - it->position) +
			(it->used ? penaltyOffset : (0.0 * score::beats));

		if (estimatedOffset < bestEstimatedOffset) {
			bestEstimatedOffset = estimatedOffset;
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
			// TODO adjust penalty amount?
			auto penaltyBarFraction = (0.25 / beats_.size()) * score::bars;
			auto penaltyOffset = pattern_.meter.BarDuration() * penaltyBarFraction;
			qualityForThisBar_ += BeatClassification::QualityFromOffset(penaltyOffset);
			it->used = true;
		}
	}
}

BarProgressEstimator::OffsetPair
BarProgressEstimator::EstimateForNextBar(OffsetPair const & positions)
{
	auto barLength = pattern_.meter.BarDuration() * score::bar;
	auto offset = positions.absolute - barLength;
	auto estimationOffset = positions.estimation - barLength;
	return OffsetPair(offset, estimationOffset);
}

} // namespace ScoreFollower
} // namespace cf
