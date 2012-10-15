#include "BeatScorer.h"

#include "cf/globals.h"
#include "cf/units_math.h"

#include "Data/BeatPattern.h"

namespace cf {
namespace ScoreFollower {

namespace bu = boost::units;

BeatScorer::BeatScorer(Data::BeatPattern const & pattern, unsigned index)
{
	assert(index < pattern.beats.size());

	auto beat = pattern.beats[index];
	position_ = beat.time * score::beats;

	if (index == 0) {
		auto pos = pattern.beats.back().time * score::beats;
		previous_ = pos - pattern.meter.BarDuration() * score::bar;
	} else {
		previous_ = pattern.beats[index - 1].time * score::beats;
	}

	interval_ = position_ - previous_;
	auto barLength = pattern.meter.BarDuration() * score::bar;
	penaltyBase_ = -(barLength / static_cast<double>(pattern.beats.size())).value();
}

BeatScorer::score_t
BeatScorer::ScoreForBeat(beat_pos_t pos) const
{
	//LOG("** Scoring: pos: %1%, ref: %2%, score: %3%", pos, position_,  -TempoChangeFraction(pos));
	return -std::abs((pos - position_).value());
	//return -TempoChangeFraction(pos);
}

BeatScorer::score_t
BeatScorer::BarPenaltyForMissed() const
{
	return 0.25 * penaltyBase_;
}

BeatScorer::score_t
BeatScorer::BeatPenaltyForUsed() const
{
	return 0.25 * penaltyBase_;
}

BeatScorer::fraction_t
BeatScorer::TempoChangeFraction(beat_pos_t pos) const
{
	if (pos < previous_) { return std::numeric_limits<fraction_t>::max(); }

	auto diff = pos - previous_;
	return std::abs(interval_ / diff - 1.0);
}

} // namespace ScoreFollower
} // namespace cf
