#include "FeatureExtractor.h"

namespace cf {
namespace FeatureExtractor {

FeatureExtractor::FeatureExtractor()
	: positionBuffer_(100)
	, dimExtractor_(positionBuffer_)
	, speedExtractor_(positionBuffer_)
{
}

bool FeatureExtractor::StartProduction()
{
	return false;
}

bool FeatureExtractor::StopProduction()
{
	return false;
}

bool FeatureExtractor::DequeueEvent(Event & result)
{
	return false;
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
	positionBuffer_.RegisterEvent(time::now(), pos);
	dimExtractor_.CalculateStuff();
	speedExtractor_.CalculateStuff();
}

} // namespace cf
} // namespace FeatureExtractor
