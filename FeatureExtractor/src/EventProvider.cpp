#include "FeatureExtractor/EventProvider.h"

#include "FeatureExtractor.h"

namespace cf {
namespace FeatureExtractor {

EventProvider * EventProvider::Create()
{
	return new FeatureExtractor();
}

} // namespace FeatureExtractor
} // namespace cf
