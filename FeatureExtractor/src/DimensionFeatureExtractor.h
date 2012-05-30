#pragma once

#include "cf/cf.h"

#include "types.h"

namespace cf {
namespace FeatureExtractor {

class DimensionFeatureExtractor
{
public:
	DimensionFeatureExtractor(EventBuffer<Point3D, timestamp_t> const & eventBuffer);

	void CalculateStuff();

private:
	EventBuffer<Point3D, timestamp_t> const & positionBuffer_;
};

} // namespace cf
} // namespace FeatureExtractor
