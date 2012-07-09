#pragma once

#include "cf/EventBuffer.h"
#include "cf/geometry.h"
#include "cf/time.h"

#include "FeatureExtractor/Extractor.h"

namespace cf {
namespace FeatureExtractor {

typedef EventBuffer<Point3D, timestamp_t> PositionBuffer;
typedef EventBuffer<Point3D, timestamp_t> SpeedBuffer;

// Visible to the outside, needs to be defined this way around
typedef Extractor::EventSignal EventSignal;
	
} // namespace FeatureExtractor
} // namespace cf
