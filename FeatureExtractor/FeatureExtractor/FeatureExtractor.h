#pragma once

#include "cf/cf.h"
#include "MotionTracker/HandObserver.h"

#include "EventBuffer.h"
#include "DimensionFeatureExtractor.h"
#include "SpeedFeatureExtractor.h"

namespace cf {
namespace FeatureExtractor {

class FeatureExtractor : public MotionTracker::HandObserver
{
public:
	FeatureExtractor();

public: // HandObserver implementation
	void HandFound();
	void HandLost();
	void NewHandPosition(float time, Point3D const & pos);

private:
	EventBuffer<Point3D, timestamp_t> positionBuffer_;
	DimensionFeatureExtractor dimExtractor_;
	SpeedFeatureExtractor speedExtractor_;
};

} // namespace cf
} // namespace FeatureExtractor
