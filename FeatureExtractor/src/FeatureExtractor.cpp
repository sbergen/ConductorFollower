#include "FeatureExtractor.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace cf {
namespace FeatureExtractor {

namespace si = boost::units::si;

boost::shared_ptr<Extractor>
Extractor::Create()
{
	return boost::make_shared<FeatureExtractor>();
}

FeatureExtractor::FeatureExtractor()
	: positionBuffer_(512)
	, gestureBuffer_(128)

	, previousBeat_(timestamp_t::min())

	, dimExtractor_(positionBuffer_)
	, speedExtractor_(positionBuffer_)
{
	speedExtractor_.BeatDetected.connect(BeatDetected);
	speedExtractor_.BeatDetected.connect(boost::bind(&FeatureExtractor::UpdateLatestBeat, this, _1));

	speedExtractor_.ApexDetected.connect(ApexDetected);
}

FeatureExtractor::~FeatureExtractor()
{
}

void
FeatureExtractor::RegisterPosition(timestamp_t const & time, Point3D const & pos)
{
	positionBuffer_.RegisterEvent(time, pos);
	dimExtractor_.Update();
	speedExtractor_.Update();
	DetectStartGesture();
}

void
FeatureExtractor::UpdateLatestBeat(timestamp_t const & time)
{
	previousBeat_ = time;
}

void
FeatureExtractor::DetectStartGesture()
{
	if (StartGestureDetected.empty()) { return; }

	// TODO move all constants elsewhere!

	if (previousBeat_ == timestamp_t::min()) { return; }

	// Check apexes
	speedExtractor_.ApexesSince(previousBeat_, gestureBuffer_);
	auto apexes = gestureBuffer_.AllEvents();
	if (apexes.Empty()) { return; }

	// Check y-movement magnitude
	auto magnitude = MagnitudeOfMovementSince(previousBeat_);
	if (magnitude.get_y() < coord_t(20 * si::centi * si::meters)) { return; }

	// Check duration
	duration_t gestureLength = apexes[0].timestamp - previousBeat_;
	seconds_t minTempo(60.0 / 40 / 2);
	seconds_t maxTempo(60.0 / 200 / 2);
	if (gestureLength > minTempo || gestureLength < maxTempo) { return; }

	// Done!
	timestamp_t estimate = apexes[0].timestamp + gestureLength;
	StartGestureDetected(previousBeat_, estimate);
}

} // namespace FeatureExtractor
} // namespace cf
