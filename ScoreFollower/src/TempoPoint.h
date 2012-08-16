#pragma once

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class TempoPoint
{
public:
	TempoPoint() {} // Allow uninitialized ctor
	TempoPoint(score_time_t const & ref, beat_pos_t const & position, tempo_t const & tempo)
		: reference_(ref), position_(position), tempo_(tempo) {}

	beat_pos_t const & position() const { return position_; }
	tempo_t const & tempo() const { return tempo_; }

	beats_t BeatsTo(score_time_t const & time) const
	{
		return (time - reference_) * tempo_;
	}

	beat_pos_t PositionAt(score_time_t const & time) const
	{
		return position_ + BeatsTo(time);
	}

private:
	score_time_t reference_;
	beat_pos_t position_;
	tempo_t tempo_;
};

} // namespace ScoreFollower
} // namespace cf