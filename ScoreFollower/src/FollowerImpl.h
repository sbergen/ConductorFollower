#pragma once

#include <utility>

#include <boost/scoped_ptr.hpp>
#include <boost/signals2.hpp>

#include "cf/Logger.h"

#include "MotionTracker/Event.h"
#include "FeatureExtractor/Extractor.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerOptions.h"
#include "ScoreFollower/FollowerStatus.h"
#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"
#include "ScoreFollower/ScoreReader.h"

#include "AudioBlockTimeManager.h"
#include "globals.h"
#include "ScoreHelper.h"
#include "TimeWarper.h"
#include "TempoFollower.h"

namespace cf {

namespace MotionTracker {
	class EventProvider;
	class EventThrottler;
} // namespace MotionTracker 

namespace ScoreFollower {

class FollowerImpl : public Follower
{
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
	unsigned ScoreTimeToFrameOffset(score_time_t const & time) const;
	double NewVelocityAt(double oldVelocity, score_time_t const & time) const;

	void GotStartGesture(real_time_t const & beatTime, real_time_t const & startTime);
	void GotBeat(real_time_t const & time);

	void ConsumeEvent(MotionTracker::Event const & e);

	void HandleNewPosition(real_time_t const & timestamp);
	void UpdateMagnitude(real_time_t const & timestamp);

private:
	// Temporarily private, until GUI is updated to support enums
	enum State
	{
		WaitingForStartGesture = 0,
		Rolling,
		Stopped
	};

	State state_;

private:
	GlobalsInitializer globalsInit_;

	Status::FollowerStatus status_;
	Options::FollowerOptions options_;

	// Created via shared_ptr
	boost::shared_ptr<MotionTracker::EventProvider> eventProvider_;
	boost::scoped_ptr<MotionTracker::EventThrottler> eventThrottler_;
	boost::shared_ptr<FeatureExtractor::Extractor> featureExtractor_;

	AudioBlockTimeManager timeManager_;
	TimeWarper timeWarper_;
	TempoFollower tempoFollower_;
	ScoreHelper scoreHelper_;

	real_time_t startRollingTime_;
	speed_t previousSpeed_;
	double velocity_;

	std::pair<score_time_t, score_time_t> scoreRange_;

	typedef boost::signals2::connection SignalConnection;

	SignalConnection startGestureConnection_;
	SignalConnection beatConnection_;
};

} // namespace ScoreFollower
} // namespace cf
