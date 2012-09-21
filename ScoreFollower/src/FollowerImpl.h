#pragma once

#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/lockfree/ringbuffer.hpp>

#include "cf/globals.h"
#include "cf/ButlerThread.h"

#include "MotionTracker/Event.h"

#include "PatchMapper/ConductorContext.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerOptions.h"
#include "ScoreFollower/FollowerStatus.h"
#include "ScoreFollower/types.h"
#include "ScoreFollower/TrackReader.h"
#include "ScoreFollower/ScoreReader.h"
#include "ScoreFollower/StatusEvents.h"

#include "AudioBlockTimeManager.h"
#include "TempoFollower.h"

namespace cf {

namespace MotionTracker {
	class EventProvider;
	class EventThrottler;
} // namespace MotionTracker 

namespace ScoreFollower {

class ScoreHelper;
class TimeHelper;

struct StatusEventProviderImpl : public StatusEventProvider
{
	StatusEventProviderImpl()
		: buffer_(32)
	{}

	bool DequeueEvent(StatusEvent & result)
	{
		return buffer_.dequeue(result);
	}

	typedef boost::lockfree::ringbuffer<StatusEvent, 0> Buffer;
	Buffer buffer_;
};

class FollowerImpl : public Follower
{
private:
	// Keep this at the very top to ensure it's constructed first and destructed last
	GlobalsRef globalsRef_;

public:
	FollowerImpl(boost::shared_ptr<ScoreReader> scoreReader);
	~FollowerImpl();

public: // Follower implementation
	StatusBuffer & status() { return statusBuffer_; }
	OptionsBuffer & options() { return optionsBuffer_; }
	MotionTracker::EventProvider & eventProvider() { return *eventProvider_; }
	StatusEventProvider & statusEventProvider() { return statusEventProvider_; }

	// Called from non-rt context
	void SetBlockParameters(unsigned samplerate, unsigned blockSize);

	// These are called from RT context
	unsigned StartNewBlock();
	void GetTrackEventsForBlock(unsigned track, BlockBuffer & events);

private:
	FollowerState State();
	void SetState(FollowerState::Value state, bool propagateChange = true);

	void ConsumeEvent(MotionTracker::Event const & e);

private: // Stuff related to butler thread
	void CheckForConfigChange();
	void CollectData(std::string const & scoreFile);
	void RestartScore();
	void EnsureMotionTrackingIsStarted();

private:
	typedef boost::signals2::connection      SignalConnection;
	typedef boost::mutex::scoped_try_lock    TryLock;
	typedef boost::unique_lock<boost::mutex> Lock;

private: // Change tracking
	int restartVersion_;
	std::string scoreFile_; // Used from ctor and butler

private:
	Status::FollowerStatus status_;
	StatusBuffer statusBuffer_;

	OptionsBuffer optionsBuffer_;
	OptionsBuffer::Reader optionsReader_;

	FollowerState state_;
	
	boost::mutex configMutex_;

	PatchMapper::ConductorContext conductorContext_;

	boost::shared_ptr<ScoreReader> scoreReader_;
	boost::shared_ptr<MotionTracker::EventProvider> eventProvider_;
	boost::shared_ptr<MotionTracker::EventThrottler> eventThrottler_;
	boost::shared_ptr<TimeHelper> timeHelper_;
	boost::shared_ptr<ScoreHelper> scoreHelper_;

	StatusEventProviderImpl statusEventProvider_;

	// Destroy this very first
	ButlerThread::CallbackHandle configCallbackHandle_;
};

} // namespace ScoreFollower
} // namespace cf
