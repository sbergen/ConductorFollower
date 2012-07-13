#pragma once

#include <vector>

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

	void AverageVelocityForTimespans(Velocity3D & total, std::vector<Velocity3D> & segments, std::vector<timestamp_t> const & times);

	Velocity3D AverageVelocitySince(timestamp_t const & time);

private:
	void UpdateVelocityBuffer();

private:
	PositionBuffer const & positionBuffer_;
	VelocityBuffer speedBuffer_;
	
	GestureBuffer beatBuffer_;
	GestureBuffer apexBuffer_;
	Velocity3D prevAvgSeed_;
};

} // namespace FeatureExtractor
} // namespace cf
