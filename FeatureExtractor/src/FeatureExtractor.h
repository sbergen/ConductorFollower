#pragma once

#include "cf/cf.h"
#include "MotionTracker/HandObserver.h"

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

public: // HandObserver implementation
	void HandFound();
	void HandLost();
	void NewHandPosition(float time, Point3D const & pos);

private:
	PositionBuffer positionBuffer_;
	DimensionFeatureExtractor dimExtractor_;
	SpeedFeatureExtractor speedExtractor_;
};

} // namespace cf
} // namespace FeatureExtractor
