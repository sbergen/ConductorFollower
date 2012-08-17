#pragma once

#include <vector>

#include <boost/utility.hpp>
#include <boost/variant.hpp>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/ScoreReader.h"

#include "ScorePosition.h"

namespace cf {
namespace ScoreFollower {

class TimeSignature;

class TempoMap : public boost::noncopyable
{
public:
	TempoMap();

	void ReadScore(ScoreReader & reader);

	ScorePosition GetScorePositionAt(score_time_t const & time) const;
	TimeSignature GetMeterAt(score_time_t const & time) const;

private:
	void ReadTempo(TempoReaderPtr reader);
	void ReadMeter(MeterReaderPtr reader);
	void EnsureChangesNotEmpty();

private:

	typedef EventBuffer<ScorePosition, score_time_t, std::vector> ChangeMap;

	ChangeMap changes_;
};

} // namespace ScoreFollower
} // namespace cf
