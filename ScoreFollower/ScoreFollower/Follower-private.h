#pragma once

#include <utility>

#include <boost/scoped_ptr.hpp>

#include "cf/Logger.h"

#include "FeatureExtractor/Event.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"

namespace cf {

namespace FeatureExtractor {
	class EventProvider;
} // namespace FeatureExtractor

namespace ScoreFollower {

class GlobalsInitializer;
class TimeWarper;
class TempoFollower;
class AudioBlockTimeManager;

// One class of indirection to make the implementation private
// This basically implments all the template parameter
// independent parts of Follower
class FollowerTypeIndependentImpl
{
private: // Only accessible by Follower
	template <class TData> friend class Follower;

	FollowerTypeIndependentImpl(unsigned samplerate, unsigned blockSize);
	~FollowerTypeIndependentImpl();

	void ReadTempoTrack(TrackReader<tempo_t> & reader);
	void StartNewBlock(std::pair<score_time_t, score_time_t> & scoreRange);

	unsigned ScoreTimeToFrameOffset(score_time_t const & time);
	bool Rolling() const { return rolling_; }

private:
	void EnsureProperStart();
	void ConsumeEvents();
	void ConsumeEvent(FeatureExtractor::Event const & e);

private:
	boost::scoped_ptr<GlobalsInitializer> globalsInit_;
	boost::scoped_ptr<FeatureExtractor::EventProvider> eventProvider_;
	boost::scoped_ptr<AudioBlockTimeManager> timeManager_;
	boost::scoped_ptr<TimeWarper> timeWarper_;
	boost::scoped_ptr<TempoFollower> tempoFollower_;

	bool started_;
	bool rolling_;
	speed_t previousSpeed_;

	struct QueuedEvent
	{
		QueuedEvent() : isQueued(false) {}
		bool isQueued;
		FeatureExtractor::Event e;
	};
	QueuedEvent queuedEvent_;
};

} // namespace ScoreFollower
} // namespace cf
