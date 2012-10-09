#pragma once

#include "cf/ChenBuffer.h"
#include "cf/score_units.h"

namespace cf {
namespace MotionTracker {

struct MusicalContext
{
	boost::units::quantity<score::tempo> currentTempo;
	boost::units::quantity<score::tempo> scoreTempo;

	bool rolling;
};

typedef ChenBuffer<MusicalContext, 1> MusicalContextBuffer;

} // namespace MotionTracker
} // namespace cf
