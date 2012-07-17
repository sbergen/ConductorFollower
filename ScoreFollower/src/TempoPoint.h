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

	beats_t BeatsTo(score_time_t time) const { return (time - reference_) * tempo_; }

	/*
	// whole beat portion of time at this tempo, TODO FIX!!!
	beats_t wholeBeats(score_time_t time) const { return beats_t((time - reference_) / tempo_); }
		
	// remainder (not including whole beats) of time at this tempo
	score_time_t remainder(score_time_t time) const { return (time - reference_) % tempo_; }
		
	// remainder translated to beats at this tempo
	beat_pos_t fraction(score_time_t time) const
	{
		return static_cast<beat_pos_t>(remainder(time).count()) / tempo_.count();
	}
	*/

private:
	score_time_t reference_;
	beat_pos_t position_;
	tempo_t tempo_;
};

} // namespace ScoreFollower
} // namespace cf