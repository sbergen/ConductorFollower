#include "FeatureExtractor.h"

namespace cf {
namespace FeatureExtractor {

FeatureExtractor::FeatureExtractor()
	: positionBuffer_(100)
	, dimExtractor_(positionBuffer_)
	, speedExtractor_(positionBuffer_)
{
}

void
FeatureExtractor::HandFound()
{

}

void
FeatureExtractor::HandLost()
{

}

void
FeatureExtractor::NewHandPosition(float time, Point3D const & pos)
{
	positionBuffer_.RegisterEvent(time, pos);
	dimExtractor_.CalculateStuff();
	speedExtractor_.CalculateStuff();
}

} // namespace cf
} // namespace FeatureExtractor
