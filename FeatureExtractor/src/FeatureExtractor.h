#pragma once

#include <boost/scoped_ptr.hpp>

#include "cf/cf.h"
#include "cf/LockfreeThread.h"
#include "MotionTracker/HandObserver.h"
#include "MotionTracker/HandTracker.h"

#include "FeatureExtractor/EventProvider.h"

#include "types.h"
#include "DimensionFeatureExtractor.h"
#include "SpeedFeatureExtractor.h"

namespace cf {
namespace FeatureExtractor {

class FeatureExtractor : public EventProvider, public MotionTracker::HandObserver
{
public:
	FeatureExtractor();

public: // EventProvider implementation
	bool StartProduction();
	bool StopProduction();
	bool DequeueEvent(Event & result);

public: // HandObserver implementation, called from tracker thread
	void HandFound();
	void HandLost();
	void NewHandPosition(float time, Point3D const & pos);

private: // Actual feature extraction
	PositionBuffer positionBuffer_;
	DimensionFeatureExtractor dimExtractor_;
	SpeedFeatureExtractor speedExtractor_;

private: // tracker thread state and event buffer
	bool Init();
	bool EventLoop();

	LockfreeThread trackerThread_;
	boost::scoped_ptr<MotionTracker::HandTracker> tracker_;
	InterThreadEventBuffer eventBuffer_;
};

} // namespace cf
} // namespace FeatureExtractor
