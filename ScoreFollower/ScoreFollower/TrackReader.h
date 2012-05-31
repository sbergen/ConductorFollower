#pragma once

namespace cf {
namespace ScoreFollower {

// Abstraction of reading a score (e.g. midi)
template<typename TData>
class TrackReader
{
public:
	virtual ~TrackReader() {}

	virtual bool NextEvent(score_time_t & timestamp, TData & data) = 0;
};

} // namespace ScoreFollower
} // namespace cf
