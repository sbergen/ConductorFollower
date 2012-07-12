#pragma once

#include <vector>

#include <boost/utility.hpp>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/ScoreReader.h"

#include "TempoPoint.h"

namespace cf {
namespace ScoreFollower {

class TimeSignature;

class TempoMap : public boost::noncopyable
{
public:
	TempoMap();

	void ReadScore(ScoreReader & reader);

	TempoPoint GetTempoAt(score_time_t const & time) const;
	TimeSignature GetMeterAt(score_time_t const & time) const;

private:
	void ReadTempo(TempoReaderPtr reader);
	void ReadMeter(MeterReaderPtr reader);
	void EnsureChangesNotEmpty();

private:
	class TempoChange
	{
	public:
		TempoChange() {} // Allow uninitialized ctor
		TempoChange(score_time_t const & timestamp, TempoPoint const & tempo);
		TempoPoint GetTempoAt(score_time_t const & time) const;

	private:
		score_time_t timestamp_;
		TempoPoint tempo_;
	};

	typedef EventBuffer<TempoChange, score_time_t, std::vector> ChangeMap;
	typedef EventBuffer<TimeSignature, score_time_t, std::vector> MeterMap;

	ChangeMap changes_;
	MeterMap  meters_;
};

} // namespace ScoreFollower
} // namespace cf
