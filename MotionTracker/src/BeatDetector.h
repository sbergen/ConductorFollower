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

	bool Detect(MotionState const & state, double & strength);

private:
	void ResetBottom();

private:

	//typedef SavitzkyGolayPeakDetector<3, 2> PeakDetector;
	//PeakDetector peakDetector_;

	AveragingFir<3> vyFir_;
	double prevVy_;

	Point3D bottomPos_;
};

} // namespace MotionTracker
} // namespace cf
