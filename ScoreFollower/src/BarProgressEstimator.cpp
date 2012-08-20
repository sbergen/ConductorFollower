#include "BarProgressEstimator.h"

#include "BeatClassification.h"

namespace cf {
namespace ScoreFollower {

BarProgressEstimator::BarProgressEstimator(Data::BeatPattern const & pattern)
	: pattern_(pattern)
	, qualityForThisBar_(0.0)
{
	beats_.reserve(pattern_.beats.size() * 2);
}

BeatClassification
BarProgressEstimator::ClassifyBeat(ScorePosition const & position, beat_pos_t beginningOfBar)
{
	// Naive implementation
	if (beginningOfBar != previousBeginningOfBar_) {
		previousBeginningOfBar_ = beginningOfBar;
		beats_.clear();
		qualityForThisBar_ = 0.0;
	}

	beat_pos_t relativePos = position.position() - beginningOfBar;
	LOG("Relative pos: %1%", relativePos);

	if (beats_.size() >= pattern_.beats.size() ||
		// TODO This part is really ugly!
		relativePos > (1.5 * pattern_.meter.BarDuration() * score::bar))
	{
		auto offset = relativePos - pattern_.meter.BarDuration() * score::bar;
		qualityForThisBar_ += BeatClassification::QualityFromOffset(offset);
		return BeatClassification(position, BeatClassification::NextBar, offset, qualityForThisBar_);
	} else {
		auto i = beats_.size();
		auto expectedBeatPos = pattern_.beats[i].time * score::beats;
		auto offset = relativePos - expectedBeatPos;
		qualityForThisBar_ += BeatClassification::QualityFromOffset(offset);

		beats_.push_back(position);
		return BeatClassification(position, BeatClassification::CurrentBar, offset, qualityForThisBar_);
	}
}


} // namespace ScoreFollower
} // namespace cf
