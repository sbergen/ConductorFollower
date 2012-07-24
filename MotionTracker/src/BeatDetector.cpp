#include "BeatDetector.h"

#include "cf/math.h"

#include "FrameRateDependent.h"

namespace cf {
namespace MotionTracker {

BeatDetector::BeatDetector()
	: peakDetector_(4)
{
	// TODO these are framerate dependent
	vyFir_[0] = -1.0;
	vyFir_[1] = -1.0;
	vyFir_[2] = -1.0;
	vyFir_[3] = 1.0;
	vyFir_[4] = 2.0;
	vyFir_[5] = 3.0;
}

double
BeatDetector::ValFromState(MotionState const & state)
{
	// Calculate a_t
	//auto prod = ublas::inner_prod(state.acceleration.data(), state.velocity.data());
	//auto a_t = acceleration_t::from_value(prod / geometry::abs(state.velocity).value());

	double vy = state.velocity.get_raw<coord::Y>();
	double val = vyFir_.Run(vy);
	if (peakDetector_.Run(val) == PeakDetector::Max &&
		val > 0.7) {
		return val;
	}

	return 0.0;
}

} // namespace MotionTracker
} // namespace cf
