#pragma once

#include "cf/units_math.h"

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
		, absolutePosition_(0.0 * score::beats)
		, tempo_(tempo)
		, meter_(meter)
		, bar_(0.0 * score::bars)
		, beat_(0.0 * score::beats)
	{}

	ScorePosition ChangeAt(score_time_t const & time, TimeSignature const & meter, tempo_t const & tempo) const
	{
		auto barDuration = meter.BarDuration();
		bars_t barsFromBeginningOfThisBar((BeatsTo(time) + beat_) / barDuration);
		bars_t wholeBars = boost::units::floor(barsFromBeginningOfThisBar);
		beats_t beat((barsFromBeginningOfThisBar - wholeBars) * barDuration);
		beat_pos_t position = AbsolutePositionAt(time);

		return ScorePosition(time, position, tempo, meter, bar_ + wholeBars, beat);
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

	beat_pos_t const & position() const { return absolutePosition_; }
	tempo_t const & tempo() const { return tempo_; }
	TimeSignature const & meter() const { return meter_; }
	bar_pos_t const & bar() const { return bar_; }
	beat_pos_t const & beat() const { return beat_; }

private: // "explicit" construction is private, use the generation methods otherwise
	ScorePosition(score_time_t const & ref, beat_pos_t const & position,
	              tempo_t const & tempo, TimeSignature const & meter,
	              bar_pos_t bar, beat_pos_t beat)
		: reference_(ref)
		, absolutePosition_(position)
		, tempo_(tempo)
		, meter_(meter)
		, bar_(bar)
		, beat_(beat)
	{}

	beats_t BeatsTo(score_time_t const & time) const
	{
		return (time - reference_) * tempo_;
	}

	beat_pos_t AbsolutePositionAt(score_time_t const & time) const
	{
		return absolutePosition_ + BeatsTo(time);
	}

private:
	score_time_t reference_;
	beat_pos_t absolutePosition_;
	tempo_t tempo_;
	TimeSignature meter_;
	bar_pos_t bar_;
	beat_pos_t beat_;
};

} // namespace ScoreFollower
} // namespace cf