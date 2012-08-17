#pragma once

#include "ScoreFollower/types.h"
#include "ScoreFollower/TimeSignature.h"

namespace cf {
namespace ScoreFollower {

class ScorePosition
{
public: // Constructors and generation methods

	// Position at beginning, with MIDI defaults
	ScorePosition(tempo_t const & tempo = tempo_t(120.0 * score::beats_per_minute),
	              TimeSignature const & meter = TimeSignature(4, 4))
		: reference_(0.0 * score::seconds)
		, position_(0.0 * score::beats)
		, tempo_(tempo)
		, meter_(meter)
	{}

	ScorePosition ChangeAt(score_time_t const & time, TimeSignature const & meter, tempo_t const & tempo) const
	{
		beat_pos_t position = BeatPositionAt(time);
		return ScorePosition(time, position, tempo, meter);
	}

	ScorePosition ChangeAt(score_time_t const & time, tempo_t const & tempo, TimeSignature const & meter) const
	{
		return ChangeAt(time, meter, tempo);
	}

	ScorePosition ChangeAt(score_time_t const & time, tempo_t const & tempo) const
	{
		return ChangeAt(time, meter_, tempo);
	}

	ScorePosition ChangeAt(score_time_t const & time, TimeSignature const & meter) const
	{
		return ChangeAt(time, meter, tempo_);
	}

	ScorePosition ScorePositionAt(score_time_t const & time) const
	{
		return ChangeAt(time, meter_, tempo_);
	}

public:

	beat_pos_t const & position() const { return position_; }
	tempo_t const & tempo() const { return tempo_; }
	TimeSignature const & meter() const { return meter_; }

	beats_t BeatsTo(score_time_t const & time) const
	{
		return (time - reference_) * tempo_;
	}

	beat_pos_t BeatPositionAt(score_time_t const & time) const
	{
		return position_ + BeatsTo(time);
	}

private: // "explicit" construction is private, use the generation methods otherwise
	ScorePosition(score_time_t const & ref, beat_pos_t const & position,
		tempo_t const & tempo, TimeSignature const & meter)
		: reference_(ref)
		, position_(position)
		, tempo_(tempo)
		, meter_(meter)
	{}

private:
	score_time_t reference_;
	beat_pos_t position_;
	tempo_t tempo_;
	TimeSignature meter_;
};

} // namespace ScoreFollower
} // namespace cf