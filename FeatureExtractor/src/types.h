#pragma once

#include <boost/lockfree/ringbuffer.hpp>

#include "cf/cf.h"
#include "cf/EventBuffer.h"

#include "FeatureExtractor/Event.h"

namespace cf {
namespace FeatureExtractor {

typedef EventBuffer<Point3D, timestamp_t> PositionBuffer;
typedef EventBuffer<Point3D, timestamp_t> SpeedBuffer;

typedef boost::lockfree::ringbuffer<Event, 0> InterThreadEventBuffer;
	
} // namespace FeatureExtractor
} // namespace cf
