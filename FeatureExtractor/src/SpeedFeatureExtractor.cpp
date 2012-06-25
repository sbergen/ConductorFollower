#include "SpeedFeatureExtractor.h"

#include <boost/geometry.hpp>
namespace bg = boost::geometry;

namespace cf {
namespace FeatureExtractor {

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
	UpdateSpeedBuffer();

	timestamp_t since = speedBuffer_.AllEvents().LastTimestamp() - milliseconds_t(100);
	auto eventsSince = speedBuffer_.EventsSince(since);
	auto data = eventsSince.DataAs<IteratorLinestring>();
	
	// First time around, for one position, we can't calculate the speed...
	if (data.size() == 0) { return; }

	// Common data
	auto zero = bg::make_zero<Point3D>();

	Point3D centroid;
	bg::centroid(data, centroid);
	//std::cout << "Average speed (directional): " << boost::geometry::dsv(centroid) << std::endl;

	coord_t absSpeedSum = std::accumulate(data.begin(), data.end(), 0.0,
		[&zero](coord_t const & sum, Point3D const & speed)
		{
			return sum + bg::distance(zero, speed);
		});
	coord_t absSpeed = absSpeedSum / data.size();
	//std::cout << "Average speed (absolute): " << absSpeed << std::endl;

	auto minMaxPair = std::minmax_element(data.begin(), data.end(),
		[&zero](Point3D const & lhs, Point3D const & rhs) -> bool
		{
			return bg::comparable_distance(zero, lhs) < bg::comparable_distance(zero, rhs);
		});
	//std::cout
	//	<< "Min speed: " << boost::geometry::dsv(*minMaxPair.first)
	//	<< ", max speed: " << boost::geometry::dsv(*minMaxPair.second) << std::endl;

	/*************************/

	if (sgn(prevAvgSeed_.get<1>()) == -1 && sgn(centroid.get<1>()) == 1 && absSpeed > 100)
	{
		timestamp_t timestamp = speedBuffer_.AllEvents().LastTimestamp() - milliseconds_t(50);
		beatBuffer_.RegisterEvent(timestamp, 1.0);
	}

	if (sgn(prevAvgSeed_.get<1>()) == 1 && sgn(centroid.get<1>()) == -1 && absSpeed > 100)
	{
		timestamp_t timestamp = speedBuffer_.AllEvents().LastTimestamp() - milliseconds_t(50);
		apexBuffer_.RegisterEvent(timestamp, 1.0);
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

void
SpeedFeatureExtractor::UpdateSpeedBuffer()
{
	timestamp_t since = speedBuffer_.AllEvents().LastTimestamp();
	auto events = positionBuffer_.EventsSince(since);

	Point3D prevPos = events.data();
	timestamp_t prevTime = events.timestamp();
	while(events.Next())
	{
		duration_t timeDiff = events.timestamp() - prevTime;
		if (timeDiff <= duration_t::zero()) {
			std::cerr << "Invalid time difference in position data!" << std::endl;
			continue;
		}

		Point3D posDiff = geometry::distance_vector(prevPos, events.data());
		bg::divide_value(posDiff, seconds_t(timeDiff).count());
		speedBuffer_.RegisterEvent(events.timestamp(), posDiff);
	}
}

} // namespace FeatureExtractor
} // namespace cf
