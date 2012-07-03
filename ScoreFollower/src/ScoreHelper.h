#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/utility.hpp>

#include "cf/EventBuffer.h"

#include "ScoreFollower/ScoreEventHandle.h"
#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class ScoreReader;

class ScoreHelper : public boost::noncopyable
{
	typedef EventBuffer<ScoreEventHandle, score_time_t, std::vector> TrackBuffer;

public:
	void CollectData(boost::shared_ptr<ScoreReader> scoreReader);
	TrackBuffer & operator[](unsigned track);

private:
	// Keep reference to scoreReader, so that the events are valid
	boost::shared_ptr<ScoreReader> scoreReader_;

	// Use a ptr_vector here, because EventBuffer is noncopyable (for a reason)
	boost::ptr_vector<TrackBuffer> trackBuffers_;
};

} // namespace ScoreFollower
} // namespace cf