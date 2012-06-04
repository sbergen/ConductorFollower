#include "ScoreFollower/FollowerPrivate.h"

#include "TimeWarper.h"

namespace cf {
namespace ScoreFollower {

FollowerTypeIndependentImpl::FollowerTypeIndependentImpl()
{
	timeWarper_.reset(new TimeWarper());
}

FollowerTypeIndependentImpl::~FollowerTypeIndependentImpl()
{
}

void
FollowerTypeIndependentImpl::ReadTempoTrack(TrackReader<tempo_t> & reader)
{
	timeWarper_->ReadTempoTrack(reader);
}

void
FollowerTypeIndependentImpl::FixTimeMapping(real_time_t const & realTime, score_time_t const & scoreTime)
{
	timeWarper_->FixTimeMapping(realTime, scoreTime);
}

void
FollowerTypeIndependentImpl::RegisterBeat(real_time_t const & beatTime)
{
	timeWarper_->RegisterBeat(beatTime);
}

score_time_t
FollowerTypeIndependentImpl::WarpTimestamp(real_time_t const & time)
{
	return timeWarper_->WarpTimestamp(time);
}

real_time_t
FollowerTypeIndependentImpl::ScoreToRealTime(real_time_t const & anchor, score_time_t const & time)
{
	return timeWarper_->InverseWarpTimestamp(anchor, time);
}

} // namespace ScoreFollower
} // namespace cf
