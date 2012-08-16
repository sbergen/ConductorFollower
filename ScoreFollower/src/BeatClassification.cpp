#include "BeatClassification.h"


namespace cf {
namespace ScoreFollower {

BeatClassification
BeatClassification::PreparatoryBeat(timestamp_t const & timestamp)
{
	BeatClassification bc(timestamp, 1.0 * score::beats, 10.0);

	bc.Evaluate(
		[](BeatClassification const & bc) -> double
		{
			if (bc.classification() == 1.0 * score::beats) { return 10.0; }
			return 0.0;
		},
		[](BeatClassification const & bc, double quality) -> double
		{
			return quality;
		});


	return bc;
}

BeatClassification::BeatClassification(timestamp_t const & timestamp, beat_pos_t rawOffset, double clarity)
	: timestamp_(timestamp)
	, rawOffset_(rawOffset)
	, clarity_(clarity)
	, classification_(rawOffset)
{}

/*
template<typename QualityEvaluator, typename SelectionFunction>
void Evaluate(QualityEvaluator const & evaluator, SelectionFunction const & selector)
{
	double bestQuality = std::numeric_limits<double>::min();
	beat_pos_t bestClassification;

	// TODO add eights
	for (int i = 0; i < NotesToClassify; ++i) {
		classification_ = (i + 1) * score::quarter_notes;
		auto offset = rawOffset_ - classification_;
		auto quality = evaluator(*this);
		classificationQualities_[i] = std::make_pair(classification_, quality);

		double selectorQuality = selector(*this, quality);
		if (selectorQuality > bestQuality) {
			bestQuality = selectorQuality;
			bestClassification = classification_;
		}
	}

	LOG("Final classification as: %1%", bestClassification);
	classification_ = bestClassification;
}
*/

double
BeatClassification::Quality() const
{
	return classification_.value() == 0.0 ? 0.0 : QualityAs(classification_);
}

double
BeatClassification::QualityAs(beat_pos_t classification) const
{
	int index = static_cast<int>(classification.value() - 1);
	assert(index < NotesToClassify);
	return classificationQualities_[index].second;
}


} // namespace ScoreFollower
} // namespace cf