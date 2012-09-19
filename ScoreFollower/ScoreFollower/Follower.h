#pragma once

#include <vector>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "cf/EventBuffer.h"
#include "cf/ChenBuffer.h"

#include "ScoreFollower/ScoreEvent.h"

namespace cf {
namespace ScoreFollower {

namespace Options { class FollowerOptions; }
namespace Status { class FollowerStatus; }

class ScoreReader;

class Follower : public boost::noncopyable
{
public:
	// Container for fetching events for each block
	typedef EventBuffer<ScoreEventPtr, samples_t> BlockBuffer;

	typedef cf::ChenBuffer<Status::FollowerStatus, 1> StatusBuffer;
	typedef cf::ChenBuffer<Options::FollowerOptions, 1> OptionsBuffer;

public: // Not real time safe functions

	// Implementation is hidden behind the factory function
	static boost::shared_ptr<Follower> Create(boost::shared_ptr<ScoreReader> scoreReader);
	virtual ~Follower() {}

	virtual StatusBuffer & status() = 0;
	virtual OptionsBuffer & options() = 0;

	virtual void SetBlockParameters(unsigned samplerate, unsigned blockSize) = 0;

public: // Real time safe functions

	// Start new audio block (blocksize defined in ctor)
	// returns number of tracks that should be read
	virtual unsigned StartNewBlock() = 0;

	// Gets events for track in current block, using the given manipulator
	virtual void GetTrackEventsForBlock(unsigned track, BlockBuffer & events) = 0;
};

} // namespace ScoreFollower
} // namespace cf
