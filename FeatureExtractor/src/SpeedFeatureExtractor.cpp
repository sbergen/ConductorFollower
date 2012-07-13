#include "SpeedFeatureExtractor.h"

#include <boost/geometry.hpp>

#include "cf/math.h"

namespace cf {
namespace FeatureExtractor {

namespace bg = boost::geometry;
namespace bu = boost::units;
namespace si = boost::units::si;

SpeedFeatureExtractor::SpeedFeatureExtractor(PositionBuffer const & eventBuffer)
	: positionBuffer_(eventBuffer)
	, speedBuffer_(128)
	, beatBuffer_(128)
	, apexBuffer_(128)
	, prevAvgSeed_(bg::make_zero<Point3D>())
{

}

void
SpeedFeatureExtractor::Update()
{
	UpdateVelocityBuffer();

	timestamp_t since = speedBuffer_.AllEvents().LastTimestamp() - milliseconds_t(100);
	auto eventsSince = speedBuffer_.EventsSince(since);
	auto data = eventsSince.DataAs<IteratorLinestring>();
	
	// First time around, for one position, we can't calculate the speed...
	if (data.size() == 0) { return; }

	// Common data
	auto zero = bg::make_zero<Velocity3D>();

	Point3D centroid;
	bg::centroid(data, centroid);

	velocity_t absSpeedSum = std::accumulate(data.begin(), data.end(), velocity_t(),
		[&zero](velocity_t const & sum, Velocity3D const & speed) -> velocity_t
		{
			Velocity3D::raw_type distance = bg::distance(zero, speed);
			return velocity_t(sum + velocity_t::from_value(distance));
		});
	velocity_t absSpeed = absSpeedSum / static_cast<double>(data.size());
	//std::cout << "Average speed (absolute): " << absSpeed << std::endl;

	/*
	auto minMaxPair = std::minmax_element(data.begin(), data.end(),
		[&zero](Point3D const & lhs, Point3D const & rhs) -> bool
		{
			return bg::comparable_distance(zero, lhs) < bg::comparable_distance(zero, rhs);
		});
	//std::cout
	//	<< "Min speed: " << boost::geometry::dsv(*minMaxPair.first)
	//	<< ", max speed: " << boost::geometry::dsv(*minMaxPair.second) << std::endl;

	/*************************/

	velocity_t v_thresh(10.0 * si::centi * si::meter / si::second);

	if (sgn(prevAvgSeed_.get_y()) == -1 && sgn(centroid.get_y()) == 1 && absSpeed > v_thresh)
	{
		timestamp_t timestamp = speedBuffer_.AllEvents().LastTimestamp() - milliseconds_t(50);
		beatBuffer_.RegisterEvent(timestamp, 1.0);
		BeatDetected(timestamp);
	}

	if (sgn(prevAvgSeed_.get_y()) == 1 && sgn(centroid.get_y()) == -1 && absSpeed > v_thresh)
	{
		timestamp_t timestamp = speedBuffer_.AllEvents().LastTimestamp() - milliseconds_t(50);
		apexBuffer_.RegisterEvent(timestamp, 1.0);
		ApexDetected(timestamp);
	}

	prevAvgSeed_ = centroid;
}

void
SpeedFeatureExtractor::BeatsSince(timestamp_t const & time, GestureBuffer & beats)
{
	beats = beatBuffer_.EventsSince(time);
}

void
SpeedFeatureExtractor::ApexesSince(timestamp_t const & time, GestureBuffer & apexes)
{
	apexes = apexBuffer_.EventsSince(time);
}

Point3D
SpeedFeatureExtractor::AverageVelocitySince(timestamp_t const & time)
{
	auto eventsSince = speedBuffer_.EventsSince(time);
	auto data = eventsSince.DataAs<IteratorLinestring>();
	Point3D centroid;
	bg::centroid(data, centroid);
	return centroid;
}

void
SpeedFeatureExtractor::UpdateVelocityBuffer()
{
	timestamp_t since = speedBuffer_.AllEvents().LastTimestamp();
	auto events = positionBuffer_.EventsSince(since);

	for (int i = 1; i < events.Size(); ++i)
	{
		auto curr = events[i];
		auto prev = events[i - 1];

		duration_t tDiff = curr.timestamp - prev.timestamp;
		if (tDiff <= duration_t::zero()) {
			std::cerr << "Invalid time difference in position data!" << std::endl;
			continue;
		}

		Point3D posDiff = geometry::distance_vector(prev.data, curr.data);
		bu::quantity<si::time> timeDiff = seconds_t(tDiff).count() * si::seconds;

		velocity_t v_x = posDiff.get_x() / timeDiff;
		velocity_t v_y = posDiff.get_y() / timeDiff;
		velocity_t v_z = posDiff.get_z() / timeDiff;

		speedBuffer_.RegisterEvent(curr.timestamp, Velocity3D(v_x, v_y, v_z));
	}
}

} // namespace FeatureExtractor
} // namespace cf
