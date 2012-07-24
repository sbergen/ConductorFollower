#pragma once

#include <boost/optional.hpp>

#include "cf/Fir.h"
#include "cf/SavitzkyGolay.h"

#include "MotionTracker/MotionState.h"

namespace cf {
namespace MotionTracker {

class BeatDetector
{
public:
	BeatDetector();

	double ValFromState(MotionState const & state);

private:

	typedef SavitzkyGolayPeakDetector<3, 2> PeakDetector;

	Fir<6> vyFir_;
	PeakDetector peakDetector_;
};




} // namespace MotionTracker
} // namespace cf
