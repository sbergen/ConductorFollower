#pragma once

#include <utility>

#include <boost/scoped_ptr.hpp>

#include "cf/Logger.h"

#include "MotionTracker/Event.h"
#include "FeatureExtractor/Extractor.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerOptions.h"
#include "ScoreFollower/FollowerStatus.h"
#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"
#include "ScoreFollower/ScoreReader.h"

namespace cf {

namespace MotionTracker {
	class EventProvider;
} // namespace MotionTracker

namespace ScoreFollower {

class GlobalsInitializer;
class TimeWarper;
class TempoFollower;
class AudioBlockTimeManager;

class FollowerImpl : public Follower
{
	//typedef Follower::BlockBuffer BlockBuffer;

public:
	FollowerImpl(unsigned samplerate, unsigned blockSize);
	~FollowerImpl();

public: // Follower implementation
	Status::FollowerStatus & status() { return status_; }
	Options::FollowerOptions & options() { return options_; }

	void CollectData(boost::shared_ptr<ScoreReader> scoreReader);
	void StartNewBlock();
	void GetTrackEventsForBlock(unsigned track, ScoreEventManipulator & manipulator, BlockBuffer & events);

private:
	void CopyEventToBuffer(score_time_t const & time, ScoreEventHandle const & data, ScoreEventManipulator & manipulator, BlockBuffer & events) const;

public:
	unsigned ScoreTimeToFrameOffset(score_time_t const & time) const;
	double NewVelocityAt(double oldVelocity, score_time_t const & time) const;
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
	real_time_t startRollingTime_;
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

	// New stuff from refactoring TODO refactor more!
	boost::shared_ptr<ScoreReader> scoreReader_;

	typedef EventBuffer<ScoreEventHandle, score_time_t, std::vector> TrackBuffer;
	std::vector<TrackBuffer> trackBuffers_;

};

} // namespace ScoreFollower
} // namespace cf
