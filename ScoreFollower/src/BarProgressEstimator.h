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

	BeatClassification ClassifyBeat(ScorePosition const & position, beat_pos_t beginningOfBar);

private:
	Data::BeatPattern pattern_;
	std::vector<ScorePosition> beats_;
	beat_pos_t previousBeginningOfBar_;

	double qualityForThisBar_;
};


} // namespace ScoreFollower
} // namespace cf
