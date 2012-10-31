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
	void RegisterCurrentScoreTempo(tempo_t newTempo) { currentScoreTempo_ = newTempo; }
	void RegisterCurrentTempo(tempo_t newTempo) { currentTempo_ = newTempo; }
	void RegisterBeat(timestamp_t const & timestamp, ScorePosition const & position, beats_t newOffset);

private:

	typedef std::multimap<TimeSignature, BeatPattern> PatternMap;

	struct BeatInfo
	{
		BeatInfo() : timestamp(timestamp_t::min()), ignore(false) {}
		BeatInfo(timestamp_t const & timestamp, ScorePosition const & position)
			: timestamp(timestamp), position(position), ignore(false) {}

		timestamp_t timestamp;
		ScorePosition position;
		BeatClassification classification;
		bool ignore;
	};

private:
	typedef bounded_vector<BeatInfo, BeatPattern::MaxBeats> BeatInfoArray;

private:
	void DiscardOldBeats();
	bool RunClassification();
	void ClassifyFirstBeat();
	bool ClassifyBeat(BeatPattern::MatchResult const & matchResult, BeatInfo & beat, int nthUnclassified);
	BeatPattern::beat_array MakeBeatArray();

private:
	TempoMap const & tempoMap_;

	PatternMap patterns_;

	TimeSignature currentTimeSignature_;
	bars_t currentBar_;
	int classifiedInCurrentBar_;
	BeatInfoArray beats_;

	ClassificationCallback callback_;

	tempo_t currentTempo_;
	tempo_t currentScoreTempo_;
};

} // namespace ScoreFollower
} // namespace cf
