#include "SpeedFeatureExtractor.h"

#include <boost/geometry.hpp>
namespace bg = boost::geometry;

namespace cf {
namespace FeatureExtractor {

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

SpeedFeatureExtractor::SpeedFeatureExtractor(EventBuffer<Point3D, timestamp_t> const & eventBuffer)
	: positionBuffer_(eventBuffer)
	, speedBuffer_(100)
	, prevAvgSeed_(bg::make_zero<Point3D>())
{

}

void
SpeedFeatureExtractor::CalculateStuff()
{
	UpdateSpeedBuffer();

	timestamp_t since = speedBuffer_.LastTimestamp() - 0.1;
	auto data = speedBuffer_.DataSince<IteratorLinestring>(since);
	
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
		std::cout << "-";
	}

	prevAvgSeed_ = centroid;
}

void
SpeedFeatureExtractor::UpdateSpeedBuffer()
{
	timestamp_t since = speedBuffer_.LastTimestamp();
	auto posData = positionBuffer_.DataSince(since);
	auto timeData = positionBuffer_.TimestampsSince(since);

	auto pos = posData.begin() + 1;
	auto time = timeData.begin() + 1;
	Point3D prevPos = posData.front();
	timestamp_t prevTime = timeData.front();
	for(/* initialized above, because limitations with the comma operator */;
		pos != posData.end() && time != timeData.end();
		++pos, ++time)
	{
		timestamp_t timeDiff = *time - prevTime;
		if (timeDiff <= 0) {
			std::cerr << "Invalid time difference in position data!" << std::endl;
			continue;
		}

		Point3D posDiff = geometry::distance_vector(prevPos, *pos);
		bg::divide_value(posDiff, timeDiff);
		speedBuffer_.RegisterEvent(*time, posDiff);
	}
}

} // namespace cf
} // namespace FeatureExtractor
