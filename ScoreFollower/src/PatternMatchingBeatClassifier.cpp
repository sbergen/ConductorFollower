#include "PatternMatchingBeatClassifier.h"

#include "cf/algorithm.h"
#include "cf/globals.h"

#include <boost/range/adaptor/transformed.hpp>

namespace cf {
namespace ScoreFollower {

PatternMatchingBeatClassifier::PatternMatchingBeatClassifier(TempoMap const & tempoMap)
	: tempoMap_(tempoMap)
{

}

void
PatternMatchingBeatClassifier::LearnPatterns(Data::PatternMap const & patternGroups)
{
	patterns_.clear();
	int totalPatterns = 0;

	for (auto gIt = patternGroups.begin(); gIt != patternGroups.end(); ++gIt) {
		auto const & patterns = gIt->second;
		for (auto pIt = patterns.begin(); pIt != patterns.end(); ++pIt) {
			patterns_.insert(std::make_pair(pIt->meter, BeatPattern(*pIt)));
			totalPatterns++;
		}
	}

	currentTimeSignature_ = tempoMap_.GetMeterAt(0.0 * score::seconds);

	LOG("Using a total of %1% beat patterns for %2% different time signatures",
		totalPatterns, (int)patternGroups.size());
}

void
PatternMatchingBeatClassifier::RegisterBeat(timestamp_t const & timestamp, ScorePosition const & position, beats_t newOffset)
{
	BeatInfo newBeat(timestamp, position);
	ClassifyWithNewBeat(newBeat);
	classifiedBeat_ = beatToClassify_;
	beatToClassify_ = newBeat;
}

void
PatternMatchingBeatClassifier::ClassifyWithNewBeat(BeatInfo const & newBeat)
{
	// TODO clean up
	static bool inited = false;
	if (classifiedBeat_.timestamp == timestamp_t::min()) { return; }
	if (!inited) {
		inited = true;
		knownClassification_ = BeatClassification(beatToClassify_.timestamp,
			beatToClassify_.position,
			// TODO type and quality are not part of the "Base"
			// classification
			BeatClassification::CurrentBar,
			0.0 * score::beats, 1.0);
	}

	auto beats = MakeBeatArray(newBeat);
	auto range = patterns_.equal_range(currentTimeSignature_);
	auto best = max_score(range.first, range.second,
		[&beats](PatternMap::const_reference pair)
		{
			return pair.second.MatchQuality(beats, 1.0);
		});

	auto const & winningPattern = best.first->second;
	auto offset = winningPattern.OffsetToBest(beatToClassify_.position.beat());

	knownClassification_ = BeatClassification(
			beatToClassify_.timestamp,
			beatToClassify_.position,
			// TODO type and quality are not part of the "Base"
			// classification
			BeatClassification::CurrentBar,
			offset, 1.0);

	if (!callback_.empty()) {
		callback_(knownClassification_);
	}
}

BeatPattern::beat_array
PatternMatchingBeatClassifier::MakeBeatArray(BeatInfo const & newBeat)
{
	auto offset = knownClassification_.offset();
	auto intendedPosition = knownClassification_.IntendedPosition();
	// If the offset is negative, we need to take it into account,
	// otherwise we get an invalid score position calculation
	auto positionDefinitelyInBar = (offset.value() > 0.0) ? knownClassification_.position().position() : intendedPosition;
	auto beginningOfBar = classifiedBeat_.position.ScorePositionAt(positionDefinitelyInBar, ScorePosition::RoundToBeat).BeginningOfThisBar();
	auto offsetBeginningOfBar = beginningOfBar + offset; // careful with the signs here...

	BeatPattern::beat_array beats;
	beats.push_back(intendedPosition - beginningOfBar);
	beats.push_back(beatToClassify_.position.position() - offsetBeginningOfBar);
	beats.push_back(newBeat.position.position() - offsetBeginningOfBar);

	return beats;
}

} // namespace ScoreFollower
} // namespace cf
