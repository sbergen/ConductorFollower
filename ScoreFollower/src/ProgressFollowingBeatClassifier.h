#pragma once

#include <map>

#include "cf/TimeSignature.h"

#include "BeatClassifier.h"
#include "BarProgressEstimator.h"
#include "TempoMap.h"

namespace cf {
namespace ScoreFollower {

class ProgressFollowingBeatClassifier : public BeatClassifier
{
public:
	ProgressFollowingBeatClassifier(TempoMap const & tempoMap);

public: // BeatClassifier implementation
	void LearnPatterns(Data::PatternMap const & patternGroups);
	BeatClassification ClassifyBeat(ScorePosition const & position, beats_t newOffset);

private:
	void ProgressToNextBar();
	void AddEstimatorsForPattern(Data::BeatPattern const & pattern);

private:
	typedef std::multimap<TimeSignature, BarProgressEstimator> EstimatorMap;

	TempoMap const & tempoMap_;
	EstimatorMap estimators_;

	TimeSignature currentTimeSignature_;
	ScorePosition currentBarStart_;
	ScorePosition nextBarStart_;

	beat_pos_t currentOffsetEstimate_;
};


} // namespace ScoreFollower
} // namespace cf
