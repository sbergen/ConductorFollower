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

public: // Module private "extensions" to Follower
	OptionsBuffer::Reader & OptionsReader() { return optionsReader_; }

private:
	void ConsumeEvent(MotionTracker::Event const & e);

private: // Stuff related to butler thread
	void CheckForConfigChange();
	void CollectData(std::string const & scoreFile);
	void EnsureMotionTrackingIsStarted();

private:
	typedef boost::signals2::connection      SignalConnection;
	typedef boost::mutex::scoped_try_lock    TryLock;
	typedef boost::unique_lock<boost::mutex> Lock;

private: // Change tracking
	std::string scoreFile_; // Used from ctor and butler

private:
	Status::FollowerStatus status_;
	StatusBuffer statusBuffer_;

	OptionsBuffer optionsBuffer_;
	OptionsBuffer::Reader optionsReader_;
	
	boost::mutex configMutex_;

	PatchMapper::ConductorContext conductorContext_;

	boost::shared_ptr<ScoreReader> scoreReader_;
	boost::shared_ptr<MotionTracker::EventProvider> eventProvider_;
	boost::shared_ptr<MotionTracker::EventThrottler> eventThrottler_;
	boost::shared_ptr<TimeHelper> timeHelper_;
	boost::shared_ptr<ScoreHelper> scoreHelper_;

	StatusEventProviderImpl statusEventProvider_;

private: // State
	class State
	{
	public:
		State(Status::FollowerStatus & status, StatusBuffer & statusBuffer);
		
		operator FollowerState::Value() const { return static_cast<FollowerState>(status_.at<Status::State>()); }
		
		bool ShouldDeliverEvents() const { return rolling_ || playback_; }
		bool TrackingStopped() const { return trackingState_ == MotionTracker::TrackingStopped; }

		void SetMotionTrackerState(MotionTracker::TrackingState trackingState, bool propagateChange = true);
		void SetHandState(MotionTracker::HandState handState, bool propagateChange = true);
		void SetRolling(bool rolling, bool propagateChange = true);
		void SetPlayback(bool playback, bool propagateChange = true);

	private:
		void UpdateState(bool propagateChange);
		FollowerState::Value ResolveState();

	private:
		Status::FollowerStatus & status_;
		StatusBuffer & statusBuffer_;

		MotionTracker::TrackingState trackingState_;
		MotionTracker::HandState::State handState_;
		bool rolling_;
		bool playback_;
	};

	State state_;

private: // Destroy this very first
	ButlerThread::CallbackHandle configCallbackHandle_;
};

} // namespace ScoreFollower
} // namespace cf
