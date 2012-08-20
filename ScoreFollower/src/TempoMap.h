#pragma once

#include <vector>

#include <boost/utility.hpp>
#include <boost/variant.hpp>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/ScoreReader.h"

#include "ScorePosition.h"

namespace cf {

class TimeSignature;

namespace ScoreFollower {

class TempoMap : public boost::noncopyable
{
public:
	TempoMap();

	void ReadScore(ScoreReader & reader);

	ScorePosition GetScorePositionAt(score_time_t const & time) const;
	TimeSignature GetMeterAt(score_time_t const & time) const;

	// timeHint needs to be smaller than the time at absolutePosition
	ScorePosition GetScorePositionAt(
		beat_pos_t const & absoluteBeatPosition,
		score_time_t timeHint = score_time_t(0.0 * score::seconds)) const;

private:
	void EnsureChangesNotEmpty();

private:

	typedef EventBuffer<ScorePosition, score_time_t, std::vector> ChangeMap;

	ChangeMap changes_;
};

} // namespace ScoreFollower
} // namespace cf
