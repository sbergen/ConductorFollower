#pragma once

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class TempoPoint
{
public:
	TempoPoint() {} // Allow uninitialized ctor
	TempoPoint(beat_pos_t position, tempo_t tempo)
		: position_(position), tempo_(tempo) {}

	beat_pos_t const & position() const { return position_; }
	tempo_t const & tempo() const { return tempo_; }

	// whole beat portion of time at this tempo
	score_time_t::rep wholeBeats(score_time_t time) const { return time / tempo_; }
		
	// remainder (not including whole beats) of time at this tempo
	score_time_t remainder(score_time_t time) const { return time % tempo_; }
		
	// remainder translated to beats at this tempo
	beat_pos_t fraction(score_time_t time) const
	{
		return static_cast<beat_pos_t>(remainder(time).count()) / tempo_.count();
	}
		
	// fraction warped to (-0.5, 0.5]. NOTE: wholeBeats will not be valid wrt the warped fraction!
	beat_pos_t warpedFraction(score_time_t time) const
	{
		beat_pos_t f = fraction(time);
		return (f > 0.5) ? (f - 1.0) : f;
	}

private:
	beat_pos_t position_;
	tempo_t tempo_;
};

} // namespace ScoreFollower
} // namespace cf