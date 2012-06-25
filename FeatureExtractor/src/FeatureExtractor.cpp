#include "FeatureExtractor.h"

namespace cf {
namespace FeatureExtractor {

Extractor * Extractor::Create()
{
	return new FeatureExtractor();
}

FeatureExtractor::FeatureExtractor()
	: positionBuffer_(128)
	, dimExtractor_(positionBuffer_)
	, speedExtractor_(positionBuffer_)
{
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
}

void
FeatureExtractor::GetBeatsSince(timestamp_t const & since, GestureBuffer & beats)
{
	speedExtractor_.BeatsSince(since, beats);
}

void
FeatureExtractor::GetApexesSince(timestamp_t const & since, GestureBuffer & apexes)
{
	speedExtractor_.ApexesSince(since, apexes);
}

Point3D
FeatureExtractor::MagnitudeOfMovementSince(timestamp_t const & time)
{
	return dimExtractor_.MagnitudeSince(time);
}

} // namespace FeatureExtractor
} // namespace cf
