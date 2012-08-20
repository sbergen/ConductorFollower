#pragma once

#include <map>

#include "cf/TimeSignature.h"

#include "Data/BeatPatternParser.h"

#include "ScoreFollower/types.h"

#include "BarProgressEstimator.h"
#include "TempoMap.h"

namespace cf {

namespace ScoreFollower {

class ScorePosition;
class BeatClassification;

class BeatClassifier
{
public:
	BeatClassifier(TempoMap const & tempoMap);
	void LearnPatterns(Data::PatternMap const & patternGroups);

	BeatClassification ClassifyBeat(ScorePosition const & position);

private:
	void ProgressToNextBar();

private:
	typedef std::multimap<TimeSignature, BarProgressEstimator> EstimatorMap;

	TempoMap const & tempoMap_;
	EstimatorMap estimators_;

	TimeSignature currentTimeSignature_;
	ScorePosition currentBarStart_;
	ScorePosition nextBarStart_;
};


} // namespace ScoreFollower
} // namespace cf
