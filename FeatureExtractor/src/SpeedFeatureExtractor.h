#pragma once

#include "cf/cf.h"

#include "types.h"

namespace cf {
namespace FeatureExtractor {

class SpeedFeatureExtractor
{
public:
	SpeedFeatureExtractor(PositionBuffer const & eventBuffer);

	void Update();
	void BeatsSince(timestamp_t const & time, GestureBuffer & beats);

private:
	void UpdateSpeedBuffer();

private:
	PositionBuffer const & positionBuffer_;
	SpeedBuffer speedBuffer_;
	
	GestureBuffer beatBuffer_;
	Point3D prevAvgSeed_;
};

} // namespace FeatureExtractor
} // namespace cf
