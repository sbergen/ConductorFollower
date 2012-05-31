#pragma once

#include "cf/cf.h"

#include "types.h"

namespace cf {
namespace FeatureExtractor {

class SpeedFeatureExtractor
{
public:
	SpeedFeatureExtractor(PositionBuffer const & eventBuffer);

	void CalculateStuff(InterThreadEventBuffer & events);

private:
	void UpdateSpeedBuffer();

private:
	PositionBuffer const & positionBuffer_;
	SpeedBuffer speedBuffer_;

	Point3D prevAvgSeed_;
};

} // namespace FeatureExtractor
} // namespace cf
