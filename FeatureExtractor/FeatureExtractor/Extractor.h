#pragma once

#include <vector>

#include "cf/cf.h"
#include "cf/EventBuffer.h"

namespace cf {
namespace FeatureExtractor {

class Extractor
{
public:
	// Hide implementation
	static Extractor * Create();

public: // types
	typedef EventBuffer<double, timestamp_t> GestureBuffer;

public:
	virtual ~Extractor() {}

	virtual void RegisterPosition(timestamp_t const & time, Point3D const & pos) = 0;

	virtual void GetBeatsSince(timestamp_t const & since, GestureBuffer & beats) = 0;

	virtual Point3D MagnitudeOfMovementSince(timestamp_t const & time) = 0;

};

} // namespace FeatureExtractor
} // namespace cf
