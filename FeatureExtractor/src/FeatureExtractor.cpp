#include "FeatureExtractor.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/geometry.hpp>
#include <boost/range/numeric.hpp>

#include "cf/globals.h"
#include "cf/math.h"
#include "cf/physics.h"

namespace cf {
namespace FeatureExtractor {

namespace bg = boost::geometry;
namespace si = boost::units::si;

boost::shared_ptr<Extractor>
Extractor::Create()
{
	return boost::make_shared<FeatureExtractor>();
}

FeatureExtractor::FeatureExtractor()
	: positionBuffer_(512)
	, velocityBuffer_(512)
	, accelerationBuffer_(512)
	, jerkBuffer_(512)
	, beatBuffer_(128)
	, apexBuffer_(128)
	, prevAvgSeed_(bg::make_zero<Velocity3D>())
{
}

FeatureExtractor::~FeatureExtractor()
{
}

void
FeatureExtractor::RegisterPosition(timestamp_t const & time,
	Point3D const & pos, Velocity3D const & vel, Acceleration3D const & acc, Jerk3D const & jerk)
{
	positionBuffer_.RegisterEvent(time, pos);
	velocityBuffer_.RegisterEvent(time, vel);
	accelerationBuffer_.RegisterEvent(time, acc);
	jerkBuffer_.RegisterEvent(time, jerk);

	DoShortTimeAnalysis();
	DetectStartGesture();
}

Point3D
FeatureExtractor::MagnitudeOfMovementSince(timestamp_t const & time)
{
	auto eventsSince = positionBuffer_.EventsSince(time);
	auto range = eventsSince.DataAs<IteratorLinestring>();

	Box3D envelope;
	bg::envelope(range, envelope);
	
	return geometry::distance_vector(envelope.min_corner(), envelope.max_corner());
}

Velocity3D
FeatureExtractor::AverageVelocitySince(timestamp_t const & time)
{
	auto positionsSince = positionBuffer_.EventsSince(time);
	if (positionsSince.Size() < 2) { return Velocity3D(); }
	return VelocityFromRange(positionsSince);
}

void
FeatureExtractor::EnvelopesForTimespans(Box3D & total, std::vector<Box3D> & segments, std::vector<timespan_t> const & times)
{
	segments.clear();
	total = bg::make_zero<Box3D>();

	if (times.empty()) { return; }

	for(int i = 0; i < times.size(); ++i) {
		auto events = positionBuffer_.EventsBetween(times[i].first, times[i].second);
		auto linestring = events.DataAs<IteratorLinestring>();
		bg::envelope(linestring, segments.back());
		bg::expand(total, segments.back());
	}
}

void
FeatureExtractor::AverageVelocityForTimespans(Velocity3D & total, std::vector<Velocity3D> & segments, std::vector<timespan_t> const & times)
{
	segments.clear();
	total = bg::make_zero<Velocity3D>();

	if (times.empty()) { return; }

	for(int i = 0; i < times.size(); ++i) {
		auto events = positionBuffer_.EventsBetween(times[i].first, times[i].second);
		segments.push_back(VelocityFromRange(events));
		bg::add_point(total, segments.back());
	}

	bg::divide_value(total, segments.size());
}

void
FeatureExtractor::DoShortTimeAnalysis()
{
	auto vEvent = velocityBuffer_.AllEvents().Back();
	Velocity3D v_avg = vEvent.data;
	Velocity3D::quantity v_abs = geometry::abs(v_avg); 
	Velocity3D::quantity v_thresh(10.0 * si::centi * si::meter / si::second);

	if (math::sgn(prevAvgSeed_.get<1>()) == -1 && math::sgn(v_avg.get<1>()) == 1 && v_abs > v_thresh)
	{
		beatBuffer_.RegisterEvent(vEvent.timestamp, 1.0);
		BeatDetected(vEvent.timestamp);
	}

	if (math::sgn(prevAvgSeed_.get<1>()) == 1 && math::sgn(v_avg.get<1>()) == -1 && v_abs > v_thresh)
	{
		apexBuffer_.RegisterEvent(vEvent.timestamp, 1.0);
		ApexDetected(vEvent.timestamp);
	}

	prevAvgSeed_ = v_avg;
}

void
FeatureExtractor::DetectStartGesture()
{
	if (StartGestureDetected.empty()) { return; }

	// TODO move all constants elsewhere!

	if (beatBuffer_.AllEvents().Empty()) { return; }
	timestamp_t previousBeat = beatBuffer_.AllEvents().LastTimestamp();

	// Check apexes
	auto apexes = apexBuffer_.EventsSince(previousBeat);
	if (apexes.Empty()) { return; }

	// Check y-movement magnitude
	auto magnitude = MagnitudeOfMovementSince(previousBeat);
	if (magnitude.get<1>() < coord_t(20 * si::centi * si::meters)) { return; }

	// Check duration
	duration_t gestureLength = apexes[0].timestamp - previousBeat;
	seconds_t minTempo(60.0 / 40 / 2);
	seconds_t maxTempo(60.0 / 200 / 2);
	if (gestureLength > minTempo || gestureLength < maxTempo) { return; }

	// Done!
	timestamp_t estimate = apexes[0].timestamp + gestureLength;
	StartGestureDetected(previousBeat, estimate);
}

Velocity3D
FeatureExtractor::VelocityFromRange(PositionBuffer::Range const & range)
{
	auto first = range.Front();
	auto last = range.Back();

	auto movement = geometry::distance_vector(first.data, last.data);
	auto duration = time::quantity_cast<time_quantity>(last.timestamp - first.timestamp);
	return movement / duration;
}

} // namespace FeatureExtractor
} // namespace cf
