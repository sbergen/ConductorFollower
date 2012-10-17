#pragma once

#include "ScoreFollower/types.h"

#include "Data/BeatPatternParser.h"

namespace cf {
namespace ScoreFollower {

class ScorePosition;
class BeatClassification;

class BeatClassifier
{
public:
	virtual ~BeatClassifier() {}
	virtual void LearnPatterns(Data::PatternMap const & patternGroups) = 0;
	virtual BeatClassification ClassifyBeat(ScorePosition const & position, beats_t newOffset) = 0;
};


} // namespace ScoreFollower
} // namespace cf
