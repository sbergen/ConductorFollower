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
	// Naive implementation

	// Reset if beginning of new bar
	if (beginningOfBar != previousBeginningOfBar_) {
		previousBeginningOfBar_ = beginningOfBar;
		for (auto it = std::begin(beats_); it != std::end(beats_); ++it) {
			it->used = false;
		}
		qualityForThisBar_ = 0.0;
	}

	// Positions
	beat_pos_t relativePos = position.position() - beginningOfBar;
	beat_pos_t estimationPos = relativePos - offsetEstimate;
	LOG("Relative position: %1%, estimation position: %2%",
		relativePos, estimationPos);

	// nearest neighbour
	auto beatIt = nearest_neighbour_linear(std::begin(beats_), std::end(beats_), estimationPos,
		[](Beat const & beat, beat_pos_t const & target)
		{
			return std::abs((beat.position - target).value());
		});

	// Offsets
	auto offset = relativePos - beatIt->position;
	auto estimationOffset = estimationPos - beatIt->position;
	LOG("Offset: %1%, estimation offset: %2%, beat already used: %3%",
		offset, estimationOffset, beatIt->used);

	// Check end of bar
	auto nextBarOffset = relativePos - pattern_.meter.BarDuration() * score::bar;
	auto nextBarEstimationOffset = estimationPos - pattern_.meter.BarDuration() * score::bar;

	bool nextBar = (beats_.back().used || boost::units::abs(nextBarEstimationOffset) < boost::units::abs(estimationOffset));
	if (nextBar) {
		qualityForThisBar_ += BeatClassification::QualityFromOffset(nextBarEstimationOffset);
		return BeatClassification(position, BeatClassification::NextBar, nextBarOffset, qualityForThisBar_);
	}

	qualityForThisBar_ += BeatClassification::QualityFromOffset(estimationOffset);
	if(!beatIt->used) {
		if (boost::units::abs(estimationOffset) < (0.4 * score::beats)) {
			beatIt->used = true;
			return BeatClassification(position, BeatClassification::CurrentBar, offset, qualityForThisBar_);
		} else {
			return BeatClassification(position, BeatClassification::NotClassified, 0.0 * score::beats, qualityForThisBar_);
		}
	} else {
		return BeatClassification(position);
	}
}


} // namespace ScoreFollower
} // namespace cf
