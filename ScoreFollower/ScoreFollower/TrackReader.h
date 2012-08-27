#pragma once

#include <boost/utility.hpp>

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

// Abstraction of reading a score (e.g. midi)
template<typename TData>
class TrackReader : public boost::noncopyable
{
public:
	virtual ~TrackReader() {}

	// Return false when no event could be read (i.e. past-the-end)
	virtual bool NextEvent(score_time_t & timestamp, TData & data) = 0;
};

} // namespace ScoreFollower
} // namespace cf
