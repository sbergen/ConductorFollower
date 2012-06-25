#pragma once

#include <utility>

#include <boost/scoped_ptr.hpp>

#include "cf/Logger.h"

#include "MotionTracker/Event.h"
#include "FeatureExtractor/Extractor.h"

#include "ScoreFollower/FollowerOptions.h"
#include "ScoreFollower/FollowerStatus.h"
#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"

namespace cf {

namespace MotionTracker {
	class EventProvider;
} // namespace MotionTracker

namespace ScoreFollower {

class GlobalsInitializer;
class TimeWarper;
class TempoFollower;
class AudioBlockTimeManager;

// One class of indirection to make the implementation private
// This basically implments all the template parameter
// independent parts of Follower
class FollowerPrivate
{
public:
	Status::FollowerStatus & status() { return status_; }
	Options::FollowerOptions & options() { return options_; }

private: // Only accessible by Follower
	template <class TData> friend class Follower;

	FollowerPrivate(unsigned samplerate, unsigned blockSize);
	~FollowerPrivate();

	void ReadTempoTrack(TrackReader<tempo_t> & reader);
	void StartNewBlock(std::pair<score_time_t, score_time_t> & scoreRange);

	unsigned ScoreTimeToFrameOffset(score_time_t const & time);
	double NewVelocityAt(double oldVelocity, score_time_t const & time);
	bool Rolling() const { return rolling_; }

private:
	void EnsureProperStart();
	void ConsumeEvents();
	void ConsumeEvent(MotionTracker::Event const & e);

	void HandleNewPosition(real_time_t const & timestamp);
	void HandleStartGesture();
	void HandlePossibleNewBeats();
	void UpdateMagnitude(real_time_t const & timestamp);

private:
	Status::FollowerStatus status_;
	Options::FollowerOptions options_;

	boost::scoped_ptr<GlobalsInitializer> globalsInit_;
	boost::scoped_ptr<MotionTracker::EventProvider> eventProvider_;
	boost::scoped_ptr<FeatureExtractor::Extractor> featureExtractor_;
	boost::scoped_ptr<AudioBlockTimeManager> timeManager_;
	boost::scoped_ptr<TimeWarper> timeWarper_;
	boost::scoped_ptr<TempoFollower> tempoFollower_;

	bool started_;
	bool rolling_;
	bool gotStartGesture_;
	
	real_time_t previousBeat_;
	speed_t previousSpeed_;
	double velocity_;

	struct QueuedEvent
	{
		QueuedEvent() : isQueued(false) {}
		bool isQueued;
		MotionTracker::Event e;
	};
	QueuedEvent queuedEvent_;

	FeatureExtractor::Extractor::GestureBuffer gestureBuffer_;

	std::pair<score_time_t, score_time_t> prevScoreRange_;
};

} // namespace ScoreFollower
} // namespace cf
