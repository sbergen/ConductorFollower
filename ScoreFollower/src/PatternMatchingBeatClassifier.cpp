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
	currentBar_ = 0.0 * score::bars;
	classifiedInCurrentBar_ = 0;

	LOG("Using a total of %1% beat patterns for %2% different time signatures",
		totalPatterns, (int)patternGroups.size());
}

void
PatternMatchingBeatClassifier::RegisterBeat(timestamp_t const & timestamp, ScorePosition const & position, beats_t newOffset)
{
	BeatInfo newBeat(timestamp, position);
	beats_.push_back(newBeat);
	DiscardOldBeats();
	RunClassification();
}

void
PatternMatchingBeatClassifier::DiscardOldBeats()
{
	auto it = beats_.cbegin();
	while (it != beats_.cend()) {
		// Keep all unclassified and unignored beats
		if (!it->classification && !it->ignore) { break; }

		// TODO fix!
		// Keep anything in the current bar and onwards
		auto bar = it->classification.IntendedBar();
		if (bar >= currentBar_) {
			break;
		}

		LOG("Discarding beat from bar %1%", bar);

		// Else erase
		it = beats_.erase(it);
	}
}

void
PatternMatchingBeatClassifier::RunClassification()
{
	// The very first beat is known
	if (beats_.size() == 1) {
		ClassifyFirstBeat();
		return;
	}

	auto beats = MakeBeatArray();
	auto range = patterns_.equal_range(currentTimeSignature_);
	auto best = max_score(range.first, range.second,
		[&beats](PatternMap::const_reference pair)
		{
			return pair.second.MatchQuality(beats, 1.0);
		});
	auto match = best.first->second.Match(beats, 1.0);

	int nthUnclassified = 0;
	for (auto it = beats_.begin(); it != beats_.end(); ++it) {
		if (it->classification) { continue; }
		if (!it->ignore) {
			ClassifyBeat(match, *it, nthUnclassified);
		}
		++nthUnclassified;
	}
}

void
PatternMatchingBeatClassifier::ClassifyFirstBeat()
{
	assert(currentBar_ == 0.0 * score::bars);

	auto & beat = beats_.front();
	beat.classification = BeatClassification(
		beat.timestamp,
		beat.position,
		BeatClassification::CurrentBar, // TODO remove these from base class...
		beat.position.position(), // This will explode if we get here in the wrong place (which is good!)
		1.0); // TODO remove this also...
	
	if (!callback_.empty()) {
		callback_(beat.classification);
	}

	classifiedInCurrentBar_ = 1;
}

void
PatternMatchingBeatClassifier::ClassifyBeat(BeatPattern::MatchResult const & matchResult, BeatInfo & beat, int nthUnclassified)
{
	LOG("Classifying beat at: %1%", beat.position.position());

	// Make classification
	auto offset = matchResult.OffsetToBest(beat.position.beat());
	BeatClassification classification(
		beat.timestamp, beat.position,
		// TODO type and quality are not part of the "Base" classification
		BeatClassification::CurrentBar,
		offset, 1.0);

	// Check for bar switch
	auto bar = classification.IntendedBar();
	if (bar > currentBar_) {
		LOG("Switching from bar %1% to bar %2%", currentBar_, bar);
		currentBar_ = bar;
		classifiedInCurrentBar_ = 0;
	}

	switch (classifiedInCurrentBar_) {
	case 0:
		// Now using previous bar beats as help
		// => classify first according to them
		break;
	case 1:		
		if (nthUnclassified > 0) { break; }
		if (matchResult.IsConfident(classification)) { break; }

		beat.ignore = true;
		LOG("Ignored beat at: %1%", beat.position.position());
		return;
	case 2:
		// All clear
		break;
	}

	beat.classification = classification;
	++classifiedInCurrentBar_;
	if (!callback_.empty()) {
		callback_(classification);
	}
}

BeatPattern::beat_array
PatternMatchingBeatClassifier::MakeBeatArray()
{
	// The first beat is the first in this bar, use it as reference
	// The last one might be in the next bar, but we don't know that yet
	auto const & firstBeat = beats_.front();
	assert(firstBeat.classification);
	auto const & firstClassification = firstBeat.classification;
	auto offsetBeginningOfBar = // careful with the signs here...
		firstClassification.IntendedBeginningOfBar() +
		firstClassification.offset();

	BeatPattern::beat_array ret;
	for (auto it = beats_.cbegin(); it != beats_.cend(); ++it) {
		ret.push_back(it->position.position() - offsetBeginningOfBar);
	}

	return ret;
}

} // namespace ScoreFollower
} // namespace cf
