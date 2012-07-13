#include "SpeedFeatureExtractor.h"

#include <boost/geometry.hpp>

#include "cf/math.h"
#include "cf/physics.h"

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
	, prevAvgSeed_(bg::make_zero<Velocity3D>())
{

}

void
SpeedFeatureExtractor::Update()
{
	//UpdateVelocityBuffer();

	// Short time analysis
	timestamp_t since = positionBuffer_.AllEvents().LastTimestamp() - milliseconds_t(100);
	Velocity3D v_avg = AverageVelocitySince(since);
	Velocity3D::quantity v_abs = geometry::abs(v_avg); 
	Velocity3D::quantity v_thresh(10.0 * si::centi * si::meter / si::second);

	if (sgn(prevAvgSeed_.get_y()) == -1 && sgn(v_avg.get_y()) == 1 && v_abs > v_thresh)
	{
		timestamp_t timestamp = since + milliseconds_t(50);
		beatBuffer_.RegisterEvent(timestamp, 1.0);
		BeatDetected(timestamp);
	}

	if (sgn(prevAvgSeed_.get_y()) == 1 && sgn(v_avg.get_y()) == -1 && v_abs > v_thresh)
	{
		timestamp_t timestamp = since + milliseconds_t(50);
		apexBuffer_.RegisterEvent(timestamp, 1.0);
		ApexDetected(timestamp);
	}

	prevAvgSeed_ = v_avg;
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

Velocity3D
SpeedFeatureExtractor::AverageVelocitySince(timestamp_t const & time)
{
	auto positionsSince = positionBuffer_.EventsSince(time);
	if (positionsSince.Size() < 2) { return Velocity3D(); }

	auto first = positionsSince.Front();
	auto last = positionsSince.Back();

	auto movement = geometry::distance_vector(first.data, last.data);
	auto duration = time::quantity_cast<time_quantity>(last.timestamp - first.timestamp);
	
	return movement / duration;
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
		auto timeDiff = time::quantity_cast<time_quantity>(tDiff);

		speedBuffer_.RegisterEvent(curr.timestamp, posDiff / timeDiff);
	}
}

} // namespace FeatureExtractor
} // namespace cf
