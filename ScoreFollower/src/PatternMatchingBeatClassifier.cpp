#include "PatternMatchingBeatClassifier.h"

#include <boost/math/distributions/skew_normal.hpp>

#include "cf/algorithm.h"
#include "cf/globals.h"

namespace cf {
namespace ScoreFollower {

namespace bm = boost::math;

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
	while (!RunClassification()) {}
}

void
PatternMatchingBeatClassifier::DiscardOldBeats()
{
	auto it = beats_.cbegin();
	while (it != beats_.cend()) {
		// Keep all unclassified and unignored beats
		if (!it->classification && !it->ignore) { break; }

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

bool
PatternMatchingBeatClassifier::RunClassification()
{
	// The very first beat is known
	if (beats_.size() == 1) {
		ClassifyFirstBeat();
		return true;
	}

	auto beats = MakeBeatArray();
	auto range = patterns_.equal_range(currentTimeSignature_);

	struct best_t
	{
		decltype(range.first) it;
		double score;
		double stretch;
	};

	best_t best = { range.second, std::numeric_limits<double>::lowest(), 1.0 };
	auto tempoFraction = currentTempo_ / currentScoreTempo_;
	auto shape = 0.5 * (1.0 - tempoFraction);
	auto dist = bm::skew_normal(1.0, 0.5, shape);

	for (auto stretch = 0.5; stretch <= 1.5; stretch += 0.025) {
		auto tempoBeingEstimated = currentTempo_ * stretch;
		//auto stretchToScore = tempoBeingEstimated / currentScoreTempo_;
		LOG("stretch: %1%, tempoBeingEstimated: %2%, scoreTempo: %3%",
			stretch, tempoBeingEstimated, currentScoreTempo_);

		auto bestForThis = max_score(range.first, range.second,
			[&](PatternMap::const_reference pair)
			{
				return pair.second.MatchQuality(beats, stretch);
			});

		auto weightedBest = std::pow(1.1, bestForThis.second);
		weightedBest *= bm::pdf(dist, stretch);
		LOG("skewness: %1%, pdf: %2%", shape, bm::pdf(dist, stretch));

		if (weightedBest >= best.score) {
			best.it = bestForThis.first;
			best.score = weightedBest;
			best.stretch = stretch;
		}
	}

	auto match = best.it->second.Match(beats, best.stretch);
	LOG("Winning stretch: %1%", best.stretch);

	int nthUnclassified = 0;
	for (auto it = beats_.begin(); it != beats_.end(); ++it) {
		if (it->classification) { continue; }
		if (!it->ignore) {
			bool success = ClassifyBeat(match, *it, nthUnclassified);
			if (!success) { return false; }
		}
		++nthUnclassified;
	}

	return true;
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

bool
PatternMatchingBeatClassifier::ClassifyBeat(BeatPattern::MatchResult const & matchResult, BeatInfo & beat, int nthUnclassified)
{
	LOG("Classifying beat at: %1%", beat.position.position());

	auto newMeter = beat.position.meter();
	if (newMeter != currentTimeSignature_)
	{
		currentTimeSignature_ = newMeter;
		return false;
	}

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
		return true;
	case 2:
		// All clear
		break;
	}

	beat.classification = classification;
	++classifiedInCurrentBar_;
	if (!callback_.empty()) {
		callback_(classification);
	}

	return true;
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
