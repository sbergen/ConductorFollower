#pragma once

#include "cf/cf.h"
#include "cf/EventBuffer.h"

namespace cf {
namespace FeatureExtractor {

typedef EventBuffer<Point3D, timestamp_t> PositionBuffer;
typedef EventBuffer<Point3D, timestamp_t> SpeedBuffer;


	
} // namespace cf
} // namespace FeatureExtractor
