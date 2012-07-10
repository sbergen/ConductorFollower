#pragma once

#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

#include "cf/globals.h"
#include "cf/ButlerThread.h"

#include "MotionTracker/Event.h"
#include "FeatureExtractor/Extractor.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerOptions.h"
#include "ScoreFollower/FollowerStatus.h"
#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"
#include "ScoreFollower/ScoreReader.h"

#include "AudioBlockTimeManager.h"
#include "ScoreHelper.h"
#include "TimeHelper.h"
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
	FollowerImpl(unsigned samplerate, unsigned blockSize, boost::shared_ptr<ScoreReader> scoreReader);
	~FollowerImpl();

public: // Follower implementation
	StatusRCU & status() { return status_; }
	OptionsRCU & options() { return options_; }

	// These are called from RT context
	unsigned StartNewBlock();
	void GetTrackEventsForBlock(unsigned track, BlockBuffer & events);

private:
	FollowerState State();
	void SetState(FollowerState::Value state);

	void GotStartGesture(real_time_t const & beatTime, real_time_t const & startTime);
	void ConsumeEvent(MotionTracker::Event const & e);

	void HandleNewPosition(real_time_t const & timestamp);
	void UpdateMagnitude(real_time_t const & timestamp);

private: // Stuff related to butler thread
	void CheckForConfigChange();
	void CollectData(std::string const & midiFile,
	                 std::string const & instrumentFile,
					 std::string const & scoreFile);

	ButlerThread::CallbackHandle configCallbackHandle_;

private:
	typedef boost::signals2::connection      SignalConnection;
	typedef boost::mutex::scoped_try_lock    TryLock;
	typedef boost::unique_lock<boost::mutex> Lock;

private:
	GlobalsRef globalsRef_;

	StatusRCU status_;
	OptionsRCU options_;
	FollowerState state_;

	boost::mutex configMutex_;
	real_time_t startRollingTime_;

	boost::shared_ptr<ScoreReader> scoreReader_;
	boost::shared_ptr<MotionTracker::EventProvider> eventProvider_;
	boost::shared_ptr<FeatureExtractor::Extractor> featureExtractor_;
	boost::shared_ptr<MotionTracker::EventThrottler> eventThrottler_;
	boost::shared_ptr<TimeHelper> timeHelper_;
	boost::shared_ptr<ScoreHelper> scoreHelper_;	

	SignalConnection startGestureConnection_;
	SignalConnection beatConnection_;
};

} // namespace ScoreFollower
} // namespace cf
