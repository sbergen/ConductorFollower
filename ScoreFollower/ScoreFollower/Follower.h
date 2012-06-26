#pragma once

#include <vector>

#include <boost/scoped_ptr.hpp>

#include "cf/EventBuffer.h"

#include "ScoreFollower/Follower-private.h"
#include "ScoreFollower/ScoreReader.h"
#include "ScoreFollower/TrackReader.h"
#include "ScoreFollower/MidiManipulator.h"

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

	Status::FollowerStatus & status() { return private_.status(); }
	Options::FollowerOptions & options() { return private_.options(); }

	void CollectData(ScoreReader<TData> & scoreReader);
	void StartNewBlock();
	void GetTrackEventsForBlock(unsigned track, MidiManipulator<TData> & manipulator, BlockBuffer & events);

private:
	// The data is copied here on purpose
	void CopyEventToBuffer(score_time_t const & time, TData data, MidiManipulator<TData> & manipulator, BlockBuffer & events);
	void ReadNormalTracks(ScoreReader<TData> & scoreReader);

private:
	typedef EventBuffer<TData, score_time_t, std::vector> TrackBuffer;
	std::vector<TrackBuffer> trackBuffers_;

	// Type independent part of implementation
	FollowerPrivate private_;
	std::pair<score_time_t, score_time_t> currentScoreBlock_;
};

#include "ScoreFollower/Follower-inl.h"

} // namespace ScoreFollower
} // namespace cf
