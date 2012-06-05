#pragma once

#include <vector>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"

namespace cf {
namespace ScoreFollower {

class TimeWarper
{
public:
	TimeWarper();

	void ReadTempoTrack(TrackReader<tempo_t> & reader);

	// TODO Remove from publi API
	void FixTimeMapping(real_time_t const & realTime, score_time_t const & scoreTime);

	void RegisterBeat(real_time_t const & beatTime);
	score_time_t WarpTimestamp(real_time_t const & time);
	real_time_t InverseWarpTimestamp(real_time_t const & reference, score_time_t const & time);

private:
	typedef double speed_t;

	speed_t CalculateSpeedAt(real_time_t time);

private:
	// class for storing warping history
	class WarpPoint
	{
	public:
		WarpPoint(speed_t speed, real_time_t realTime, score_time_t scoreTime);
		
		score_time_t Warp(real_time_t const & time) const;
		real_time_t InverseWarp(score_time_t const & time) const;
		real_time_t Timestamp() const { return realTime_; }

	private:
		speed_t speed_;
		real_time_t realTime_;
		score_time_t scoreTime_;
	};

	typedef EventBuffer<tempo_t, score_time_t, std::vector> TempoMap;
	typedef EventBuffer<WarpPoint, real_time_t> WarpHistoryBuffer;
	typedef EventBuffer<double, real_time_t> BeatHistoryBuffer;

	TempoMap tempoMap_;
	WarpHistoryBuffer warpHistory_;
	BeatHistoryBuffer beatHistory_;
};

} // namespace ScoreFollower
} // namespace cf
