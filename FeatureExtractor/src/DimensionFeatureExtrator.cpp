#include "DimensionFeatureExtractor.h"

#include <iostream>

#include <boost/geometry.hpp>
#include <boost/range/numeric.hpp>

namespace bg = boost::geometry;

namespace cf {
namespace FeatureExtractor {

DimensionFeatureExtractor::DimensionFeatureExtractor(PositionBuffer const & eventBuffer)
	: positionBuffer_(eventBuffer)
{
}

void
DimensionFeatureExtractor::CalculateStuff(InterThreadEventBuffer & events)
{
	timestamp_t since = positionBuffer_.LastTimestamp() - milliseconds_t(100);
	auto eventsSince = positionBuffer_.EventsSince(since);
	auto range = eventsSince.DataAs<IteratorLinestring>();

	Box3D envelope;
	bg::envelope(range, envelope);
	
	Point3D distance = geometry::distance_vector(envelope.min_corner(), envelope.max_corner());
	//std::cout << "Spatial magnitude of movement: " << boost::geometry::dsv(distance) << std::endl;

	Point3D centroid;
	bg::centroid(range, centroid);
	//std::cout << "Centroid of movement: " << boost::geometry::dsv(centroid) << std::endl;

	coord_t length = bg::length(range);
	//std::cout << "Length of movement: " << length << std::endl;
}

} // namespace FeatureExtractor
} // namespace cf
