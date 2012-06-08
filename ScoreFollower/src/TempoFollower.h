#pragma once

#include <vector>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"

#include "TempoMap.h"

namespace cf {
namespace ScoreFollower {

class TempoFollower
{
public:
	TempoFollower();

	void ReadTempoTrack(TrackReader<tempo_t> & reader) { tempoMap_.Read(reader); }
	void RegisterBeat(score_time_t const & beatTime);

	speed_t SpeedEstimateAt(score_time_t const & time);

private:
	typedef EventBuffer<double, score_time_t> BeatHistoryBuffer;

private:
	score_time_t AvgConductedBeatDuration(int averageOver);

private:
	TempoMap tempoMap_;
	BeatHistoryBuffer beatHistory_;

	bool newBeats_;
	speed_t speed_;
};

} // namespace ScoreFollower
} // namespace cf
