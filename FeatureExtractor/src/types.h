#pragma once

#include "cf/EventBuffer.h"
#include "cf/physics.h"
#include "cf/time.h"

#include "FeatureExtractor/Extractor.h"

namespace cf {
namespace FeatureExtractor {

typedef EventBuffer<Point3D, timestamp_t> PositionBuffer;
typedef EventBuffer<Velocity3D, timestamp_t> VelocityBuffer;
typedef EventBuffer<Acceleration3D, timestamp_t> AccelerationBuffer;
typedef EventBuffer<Jerk3D, timestamp_t> JerkBuffer;

// Visible to the outside, needs to be defined this way around
typedef Extractor::EventSignal EventSignal;
	
} // namespace FeatureExtractor
} // namespace cf
