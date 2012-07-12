#pragma once

#include <boost/utility.hpp>

#include "types.h"

namespace cf {
namespace FeatureExtractor {

class SpeedFeatureExtractor : public boost::noncopyable
{
public:
	SpeedFeatureExtractor(PositionBuffer const & eventBuffer);

	EventSignal BeatDetected;
	EventSignal ApexDetected;

	void Update();
	void BeatsSince(timestamp_t const & time, GestureBuffer & beats);
	void ApexesSince(timestamp_t const & time, GestureBuffer & apexes);
	Point3D AverageVelocitySince(timestamp_t const & time);

private:
	void UpdateSpeedBuffer();

private:
	PositionBuffer const & positionBuffer_;
	SpeedBuffer speedBuffer_;
	
	GestureBuffer beatBuffer_;
	GestureBuffer apexBuffer_;
	Point3D prevAvgSeed_;
};

} // namespace FeatureExtractor
} // namespace cf
