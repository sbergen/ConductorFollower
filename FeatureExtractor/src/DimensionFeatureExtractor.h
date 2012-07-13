#pragma once

#include <vector>

#include <boost/utility.hpp>

#include "types.h"

namespace cf {
namespace FeatureExtractor {

class DimensionFeatureExtractor : public boost::noncopyable
{
public:
	DimensionFeatureExtractor(EventBuffer<Point3D, timestamp_t> const & eventBuffer);

	void Update();

	Point3D MagnitudeSince(timestamp_t const & time);

	void EnvelopesForTimespans(Box3D & total, std::vector<Box3D> & segments, std::vector<timestamp_t> const & times);

private:
	EventBuffer<Point3D, timestamp_t> const & positionBuffer_;
};

} // namespace FeatureExtractor
} // namespace cf
