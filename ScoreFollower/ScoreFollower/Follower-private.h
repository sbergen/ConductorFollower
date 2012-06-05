#pragma once

#include <boost/scoped_ptr.hpp>

#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"

namespace cf {
namespace ScoreFollower {

class TimeWarper;

// One class of indirection to make the implementation private
// This basically implments all the template parameter
// independent parts of Follower
class FollowerTypeIndependentImpl
{
private: // Only accessible by Follower
	template <class TData> friend class Follower;

public:
	FollowerTypeIndependentImpl();
	~FollowerTypeIndependentImpl();

	void ReadTempoTrack(TrackReader<tempo_t> & reader);

	void FixTimeMapping(real_time_t const & realTime, score_time_t const & scoreTime);
	void RegisterBeat(real_time_t const & beatTime);
	
	score_time_t WarpTimestamp(real_time_t const & time);
	real_time_t ScoreToRealTime(real_time_t const & anchor, score_time_t const & time);

private: // These could be even more private...
	boost::scoped_ptr<TimeWarper> timeWarper_;
};

} // namespace ScoreFollower
} // namespace cf
