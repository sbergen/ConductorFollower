#pragma once

#include <vector>

#include "Data/BeatPattern.h"

#include "ScoreFollower/types.h"

#include "ScorePosition.h"

namespace cf {
namespace ScoreFollower {

class BeatClassification;

class BarProgressEstimator
{
public:
	BarProgressEstimator(Data::BeatPattern const & pattern);

	BeatClassification ClassifyBeat(
		ScorePosition const & position, beat_pos_t beginningOfBar, beat_pos_t offsetEstimate);

private:
	struct Beat
	{
		explicit Beat(beat_pos_t const & position)
			: position(position), used(false) {}

		beat_pos_t position;
		bool used;

		bool operator==(Beat const & other) const
			{ return position == other.position; }
	};

private:
	Data::BeatPattern pattern_;
	std::vector<Beat> beats_;
	beat_pos_t previousBeginningOfBar_;

	double qualityForThisBar_;
};


} // namespace ScoreFollower
} // namespace cf
