#pragma once

#include "cf/cf.h"
#include "cf/EventBuffer.h"

namespace cf {
namespace FeatureExtractor {

class SpeedFeatureExtractor
{
public:
	SpeedFeatureExtractor(EventBuffer<Point3D, timestamp_t> const & eventBuffer);

	void CalculateStuff();

private:
	void UpdateSpeedBuffer();

private:
	EventBuffer<Point3D, timestamp_t> const & positionBuffer_;
	EventBuffer<Point3D, timestamp_t> speedBuffer_;

	Point3D prevAvgSeed_;
};

} // namespace cf
} // namespace FeatureExtractor
