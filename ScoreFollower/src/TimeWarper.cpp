#include "TimeWarper.h"

#define SPEED_RATIO 1000L

namespace cf {
namespace ScoreFollower {

TimeWarper::TimeWarper()
	: tempoBuffer_(100)
	, currentSpeed_(SPEED_RATIO)
{
}

void
TimeWarper::ReadTempoTrack(TrackReader<bool> & reader)
{
	score_time_t timestamp;
	bool beat;
	while (reader.NextEvent(timestamp, beat)) {
		tempoBuffer_.RegisterEvent(timestamp, beat);
	}
}

void
TimeWarper::FixTimeMapping(real_time_t const & realTime, score_time_t const & scoreTime)
{

}

void
TimeWarper::RegisterBeat(real_time_t const & beatTime)
{

}

score_time_t
TimeWarper::WarpTimestamp(real_time_t const & time)
{
	// TODO implement
	return score_time_t();
}

} // namespace ScoreFollower
} // namespace cf
