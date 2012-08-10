#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/lockfree/ringbuffer.hpp>

#include "cf/LockfreeThread.h"
#include "cf/time.h"
#include "cf/geometry.h"
#include "cf/globals.h"
#include "cf/fir.h"
#include "cf/EMA.h"
#include "cf/DynamicRange.h"
#include "cf/PeakHolder.h"

#include "MotionTracker/EventProvider.h"

#include "common_types.h"
#include "HandObserver.h"
#include "HandTracker.h"
#include "MotionFilter.h"
#include "BeatDetector.h"
#include "StartGestureDetector.h"

namespace cf {
namespace MotionTracker {

class EventProviderImpl : public EventProvider, public HandObserver
{
public:
	typedef boost::lockfree::ringbuffer<Event, 0> InterThreadEventBuffer;

public:
	EventProviderImpl();
	~EventProviderImpl();

public: // EventProvider implementation
	void StartProduction();
	void StopProduction();
	bool DequeueEvent(Event & result);

public: // HandObserver implementation, called from tracker thread
	void HandFound();
	void HandLost();
	void NewHandPosition(float time, Point3D const & pos);

private:
	void RunMotionFilters(timestamp_t const & timeNow, MotionState const & state);
	bool DetectBeat(timestamp_t const & timeNow, MotionState const & state);
	void DetectStartGesture(timestamp_t const & timeNow, MotionState const & state, bool beatOccurred);

private: // tracker thread state and event buffer
	GlobalsRef globalsRef_;
	class TrackerThread;

	boost::shared_ptr<LockfreeThread<TrackerThread> > trackerThread_;
	InterThreadEventBuffer eventBuffer_;
	MotionFilter motionFilter_;

	BeatDetector beatDetector_;
	StartGestureDetector startDetector_;

	EMA<4> velocityFir_;
	PeakHolder<20> velocityPeakHolder_;
	DynamicRange<20> velocityRange_;

	EMA<4> jerkFir_;
	PeakHolder<20> jerkPeakHolder_;
};

} // namespace MotionTracker
} // namespace cf
