#pragma once

#include <vector>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"

namespace cf {
namespace ScoreFollower {

class TempoMap
{
public:
	struct TempoPoint
	{
		TempoPoint() {} // Allow uninitialized ctor
		TempoPoint(beat_pos_t position, tempo_t tempo)
			: position(position), tempo(tempo) {}

		beat_pos_t position;
		tempo_t tempo;
	};

public:
	TempoMap();

	void Read(TrackReader<tempo_t> & reader);

	TempoPoint GetTempoAt(score_time_t const & time);

private:
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
	ChangeMap changes_;
};

} // namespace ScoreFollower
} // namespace cf
