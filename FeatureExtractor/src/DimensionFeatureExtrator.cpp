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
DimensionFeatureExtractor::Update()
{
	timestamp_t lastTimestamp = positionBuffer_.AllEvents().LastTimestamp();
	timestamp_t since = lastTimestamp - milliseconds_t(1000);
	auto eventsSince = positionBuffer_.EventsSince(since);
	auto range = eventsSince.DataAs<IteratorLinestring>();

	Box3D envelope;
	bg::envelope(range, envelope);
	
	Point3D distance = geometry::distance_vector(envelope.min_corner(), envelope.max_corner());
	// TODO
	//assert(events.enqueue(Event(lastTimestamp, Event::Magnitude, distance)));
	//std::cout << "Spatial magnitude of movement: " << boost::geometry::dsv(distance) << std::endl;

	Point3D centroid;
	bg::centroid(range, centroid);
	//std::cout << "Centroid of movement: " << boost::geometry::dsv(centroid) << std::endl;

	auto length = coord_t::from_value(bg::length(range));
	//std::cout << "Length of movement: " << length << std::endl;
}

Point3D
DimensionFeatureExtractor::MagnitudeSince(timestamp_t const & time)
{
	auto eventsSince = positionBuffer_.EventsSince(time);
	auto range = eventsSince.DataAs<IteratorLinestring>();

	Box3D envelope;
	bg::envelope(range, envelope);
	
	return geometry::distance_vector(envelope.min_corner(), envelope.max_corner());
}


} // namespace FeatureExtractor
} // namespace cf
