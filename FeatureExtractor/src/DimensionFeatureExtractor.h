#pragma once

#include "cf/cf.h"

#include "types.h"

namespace cf {
namespace FeatureExtractor {

class DimensionFeatureExtractor
{
public:
	DimensionFeatureExtractor(EventBuffer<Point3D, timestamp_t> const & eventBuffer);

	void Update();

private:
	EventBuffer<Point3D, timestamp_t> const & positionBuffer_;
};

} // namespace FeatureExtractor
} // namespace cf
