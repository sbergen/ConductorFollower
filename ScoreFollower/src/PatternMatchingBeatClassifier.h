#pragma once

#include <map>

#include <boost/utility.hpp>

#include "cf/TimeSignature.h"

#include "BeatClassifier.h"
#include "BeatClassification.h"
#include "BeatPattern.h"
#include "TempoMap.h"

namespace cf {
namespace ScoreFollower {

class PatternMatchingBeatClassifier : public BeatClassifier, public boost::noncopyable
{
public:
	PatternMatchingBeatClassifier(TempoMap const & tempoMap);

public: // BeatClassifier implementation
	void LearnPatterns(Data::PatternMap const & patternGroups);
	void SetClassificationCallback(ClassificationCallback callback) { callback_ = callback; }
	void RegisterBeat(timestamp_t const & timestamp, ScorePosition const & position, beats_t newOffset);

private:

	typedef std::multimap<TimeSignature, BeatPattern> PatternMap;

	struct BeatInfo
	{
		BeatInfo() : timestamp(timestamp_t::min()) {}
		BeatInfo(timestamp_t const & timestamp, ScorePosition const & position)
			: timestamp(timestamp), position(position) {}

		timestamp_t timestamp;
		ScorePosition position;
	};

private:
	void ClassifyWithNewBeat(BeatInfo const & newBeat);
	BeatPattern::beat_array MakeBeatArray(BeatInfo const & newBeat);

private:
	TempoMap const & tempoMap_;

	PatternMap patterns_;

	TimeSignature currentTimeSignature_;
	BeatClassification knownClassification_;
	BeatInfo classifiedBeat_;
	BeatInfo beatToClassify_;

	ClassificationCallback callback_;
};

} // namespace ScoreFollower
} // namespace cf
