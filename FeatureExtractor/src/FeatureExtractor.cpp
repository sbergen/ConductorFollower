#include "FeatureExtractor.h"

namespace cf {
namespace FeatureExtractor {

Extractor * Extractor::Create()
{
	return new FeatureExtractor();
}

FeatureExtractor::FeatureExtractor()
	: positionBuffer_(1024)
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

} // namespace FeatureExtractor
} // namespace cf
