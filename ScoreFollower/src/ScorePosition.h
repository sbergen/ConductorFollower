#pragma once

#include <boost/operators.hpp>

#include "cf/math.h"
#include "cf/units_math.h"
#include "cf/TimeSignature.h"

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class ScorePosition : public boost::totally_ordered<ScorePosition>
{
public:
	enum Rounding
	{
		NoRounding,
		RoundToBar,
		RoundToBeat,
		RoundToMeterDivision
	};

public: // Constructors and generation methods

	// Position at beginning, with MIDI defaults
	ScorePosition(tempo_t const & tempo = tempo_t(120.0 * score::beats_per_minute),
	              TimeSignature const & meter = TimeSignature(4, 4))
		: absoluteTime_(0.0 * score::seconds)
		, absolutePosition_(0.0 * score::beats)
		, tempo_(tempo)
		, meter_(meter)
		, bar_(0.0 * score::bars)
		, beat_(0.0 * score::beats)
	{}

	ScorePosition ChangeAt(score_time_t const & time, TimeSignature const & meter, tempo_t const & tempo) const
	{
		auto barDuration = meter_.BarDuration();
		bars_t barsFromBeginningOfThisBar((BeatsTo(time) + beat_) / barDuration);
		bars_t wholeBars = boost::units::floor(barsFromBeginningOfThisBar);
		beats_t beat((barsFromBeginningOfThisBar - wholeBars) * barDuration);
		beat_pos_t position = AbsolutePositionAt(time);

		auto ret = ScorePosition(time, position, tempo, meter, bar_ + wholeBars, beat);
		if (meter != meter_) {
			// Do some dumb rounding here to get rid of rounding errors...
			if (ret.beat_ > 0.0 * score::beats) {
				ret.beat_ = 0.0 * score::beats;
				ret.bar_ += 1.0 * score::bars;
			}
		}
		return ret;
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

	ScorePosition ScorePositionAt(beat_pos_t const & absolutePosition, Rounding rounding = NoRounding) const
	{
		// We only go toward the future
		assert(absolutePosition >= absolutePosition_);

		auto barDuration = meter_.BarDuration();
		beat_pos_t beatsFromBeginningOfThisBar = absolutePosition - BeginningOfThisBar();
		bars_t barsFromBeginningOfThisBar = boost::units::floor(beatsFromBeginningOfThisBar / barDuration);

		auto time = AbsoluteTimeAt(absolutePosition);
		bars_t bar = bar_ + barsFromBeginningOfThisBar;
		beats_t beat = beatsFromBeginningOfThisBar - (barsFromBeginningOfThisBar * barDuration);

		return ScorePosition(time, absolutePosition, tempo_, meter_, bar, beat, rounding);
	}

	ScorePosition ScorePositionAt(bar_pos_t const & bar, beat_pos_t const & beat, Rounding rounding = NoRounding)
	{
		beat_pos_t beatsFromBeginningOfBar = (bar - bar_) * meter_.BarDuration() + beat;
		beat_pos_t absolutePos = BeginningOfThisBar() + beatsFromBeginningOfBar;
		return ScorePositionAt(absolutePos, rounding);
	}

public:

	score_time_t const & time() const { return absoluteTime_; }
	beat_pos_t const & position() const { return absolutePosition_; }
	tempo_t const & tempo() const { return tempo_; }
	TimeSignature const & meter() const { return meter_; }
	bar_pos_t const & bar() const { return bar_; }
	beat_pos_t const & beat() const { return beat_; }

	beat_pos_t BeginningOfThisBar() const
	{
		return absolutePosition_ - beat_;
	}

	beat_pos_t BeginningOfNextBar() const
	{
		return BeginningOfThisBar() + (meter_.BarDuration() * score::bar);
	}

	double FractionOfBar() const
	{
		return beat_ / (meter_.BarDuration() * score::bar);
	}

public: // Ordering and comparison (totally_ordered provides the rest)
	bool operator< (ScorePosition const & other) const
	{
		// Use absolute position
		return absolutePosition_ < other.absolutePosition_;
	}

	bool operator== (ScorePosition const & other) const
	{
		// Treat really close values as equal
		return std::abs(absolutePosition_.value() - other.absolutePosition_.value()) < 0.001;
	}

private: // "explicit" construction is private, use the generation methods otherwise
	ScorePosition(score_time_t const & time, beat_pos_t const & position,
	              tempo_t const & tempo, TimeSignature const & meter,
	              bar_pos_t bar, beat_pos_t beat,
				  Rounding rounding = NoRounding)
		: absoluteTime_(time)
		, absolutePosition_(position)
		, tempo_(tempo)
		, meter_(meter)
		, bar_(bar)
		, beat_(beat)
	{
		Round(rounding);
	}

	beats_t BeatsTo(score_time_t const & time) const
	{
		return (time - absoluteTime_) * tempo_;
	}

	beat_pos_t AbsolutePositionAt(score_time_t const & time) const
	{
		return absolutePosition_ + BeatsTo(time);
	}

	score_time_t AbsoluteTimeAt(beat_pos_t const & absolutePosition) const
	{
		beats_t beatDiff = absolutePosition - absolutePosition_;
		return absoluteTime_ + (beatDiff / tempo_);
	}

	ScorePosition & Round(Rounding rounding)
	{
		switch (rounding)
		{
		case NoRounding:
			break;
		case RoundToBar:
			{
				if (beat_ == 0.0 * score::beats) { break; }

				auto diffToEnd = meter_.BarDuration() * score::bars - beat_;
				if (diffToEnd <= beat_) {
					// Round up
					bar_ += 1.0 * score::bars;
					absoluteTime_ = AbsoluteTimeAt(absolutePosition_ + diffToEnd);
					absolutePosition_ += diffToEnd;
				} else {
					// Round down
					absoluteTime_ = AbsoluteTimeAt(absolutePosition_ - beat_);
					absolutePosition_ -=  beat_;
				}

				beat_ = 0.0 * score::beats;
			}
			break;
		case RoundToBeat:
			RoundToDuration(1.0 * score::beats);
			break;
		case RoundToMeterDivision:
			RoundToDuration((1.0 / meter_.division()) * score::beats);
			break;
		}

		return *this;
	}

	void RoundToDuration(beats_t len)
	{
		double factor = score::beat / len;
		double rounded = math::round(factor * beat_.value()) / factor;

		auto corrected = beats_t::from_value(rounded);
		auto diff = corrected - beat_;
			
		absoluteTime_ = AbsoluteTimeAt(absolutePosition_ + diff);
		absolutePosition_ += diff;

		beat_ = corrected;
		auto barDuration = meter_.BarDuration() * score::bar;
		if (beat_ >= barDuration) {
			beat_ -= barDuration;
			bar_ += 1.0 * score::bars;
		}
	}

private:
	score_time_t absoluteTime_;
	beat_pos_t absolutePosition_;
	tempo_t tempo_;
	TimeSignature meter_;
	bar_pos_t bar_;
	beat_pos_t beat_;
};

} // namespace ScoreFollower
} // namespace cf