#pragma once

#include <vector>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"

#include "TempoMap.h"

namespace cf {
namespace ScoreFollower {

class TimeWarper;

class TempoFollower
{
public:
	TempoFollower(TimeWarper const & timeWarper);

	void ReadTempoTrack(TrackReader<tempo_t> & reader) { tempoMap_.Read(reader); }
	void RegisterBeat(real_time_t const & beatTime);

	speed_t SpeedEstimateAt(real_time_t const & time);

private:
	typedef EventBuffer<double, real_time_t> BeatHistoryBuffer;

private:
	TempoPoint LastBeat() const;
	tempo_t BeatLengthEstimate() const;

	speed_t SpeedFromConductedTempo(real_time_t const & now) const;
	speed_t SpeedFromBeatCatchup() const;

private:
	TimeWarper const & timeWarper_;

	TempoMap tempoMap_;
	BeatHistoryBuffer beatHistory_;

	bool newBeats_;
	speed_t speed_;
};

} // namespace ScoreFollower
} // namespace cf
