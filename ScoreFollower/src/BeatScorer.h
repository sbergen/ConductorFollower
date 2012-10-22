#pragma once

#include "Data/BeatPattern.h"

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class BeatScorer
{
public:
	typedef double fraction_t;
	typedef double score_t;

public:
	BeatScorer(Data::BeatPattern const & pattern, unsigned index);

	score_t ScoreForBeat(beat_pos_t pos) const;
	score_t PenaltyForMissed() const;
	score_t PenaltyForUsed() const;

	beats_t OffsetTo(beat_pos_t pos) const { return pos - position_; }

	bool operator==(BeatScorer const & other) const { return position_ == other.position_; }

private:
	fraction_t TempoChangeFraction(beat_pos_t pos) const;

private:
	// Beat info
	beat_pos_t position_;
	beat_pos_t previous_;
	beats_t interval_;

	// Bar related stuff
	score_t penaltyBase_;
};

} // namespace ScoreFollower
} // namespace cf
