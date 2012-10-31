#pragma once

#include <boost/function.hpp>

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

	typedef boost::function<void (BeatClassification const &)> ClassificationCallback;
	virtual void SetClassificationCallback(ClassificationCallback callback) = 0;

	virtual void RegisterCurrentScoreTempo(tempo_t newTempo) = 0;
	virtual void RegisterCurrentTempo(tempo_t newTempo) = 0;

	virtual void RegisterBeat(timestamp_t const & timestamp, ScorePosition const & position, beats_t newOffset) = 0;
};


} // namespace ScoreFollower
} // namespace cf
