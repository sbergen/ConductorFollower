#pragma once

#include <utility>

#include <boost/array.hpp>
#include <boost/function.hpp>

#include "cf/globals.h"

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class BeatClassification
{
private:
	typedef std::pair<beat_pos_t, double> ClassificationQuality;
	enum { NotesToClassify = 16 };

public:

	static BeatClassification PreparatoryBeat(timestamp_t const & timestamp);

	BeatClassification(timestamp_t const & timestamp, beat_pos_t rawOffset, double clarity);

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

	timestamp_t timestamp() const { return timestamp_; }
	double clarity() const { return clarity_; }

	beat_pos_t classification() const { return classification_; }
	beat_pos_t offset() const { return classification_ - rawOffset_; }

	double Quality() const;

	// Must not be used during evaluation
	double QualityAs(beat_pos_t classification) const;

private:
	// These are given
	real_time_t timestamp_;
	beat_pos_t rawOffset_;
	double clarity_;

	// These are calculated
	beat_pos_t classification_;
	boost::array<ClassificationQuality, NotesToClassify> classificationQualities_;
};


} // namespace ScoreFollower
} // namespace cf
