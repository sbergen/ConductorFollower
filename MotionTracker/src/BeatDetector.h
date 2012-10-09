#pragma once

#include <boost/optional.hpp>

#include "cf/Fir.h"
#include "cf/SavitzkyGolay.h"

#include "MotionTracker/MotionState.h"
#include "MotionTracker/MusicalContext.h"

namespace cf {
namespace MotionTracker {

class BeatDetector
{
public:
	BeatDetector(MusicalContextBuffer::Reader & musicalContextReader);

	bool Detect(timestamp_t const & time, MotionState const & state, double & strength);

private:
	void ResetBottom();

private:
	MusicalContextBuffer::Reader & musicalContextReader_;

	double prevVy_;

	timestamp_t bottomTime_;
	Point3D bottomPos_;
};

} // namespace MotionTracker
} // namespace cf
