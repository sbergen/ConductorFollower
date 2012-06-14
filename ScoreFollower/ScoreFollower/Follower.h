#pragma once

#include <vector>

#include <boost/scoped_ptr.hpp>

#include "cf/EventBuffer.h"

#include "ScoreFollower/Follower-private.h"
#include "ScoreFollower/ScoreReader.h"
#include "ScoreFollower/TrackReader.h"

namespace cf {
namespace ScoreFollower {

template<typename TData>
class Follower
{
public:
	// Container for fetching events for each block
	typedef EventBuffer<TData, unsigned> BlockBuffer;

public:
	Follower(unsigned samplerate, unsigned blockSize);

	Status::FollowerStatus & status() { return tiImpl_.status(); }

	void CollectData(ScoreReader<TData> & scoreReader);
	void StartNewBlock();
	void GetTrackEventsForBlock(unsigned track, BlockBuffer & events);

private:
	void ReadNormalTracks(ScoreReader<TData> & scoreReader);

private:
	typedef EventBuffer<TData, score_time_t, std::vector> TrackBuffer;
	std::vector<TrackBuffer> trackBuffers_;

	// Type independent part of implementation
	FollowerTypeIndependentImpl tiImpl_;
	std::pair<score_time_t, score_time_t> currentScoreBlock_;
};

#include "ScoreFollower/Follower-inl.h"

} // namespace ScoreFollower
} // namespace cf
