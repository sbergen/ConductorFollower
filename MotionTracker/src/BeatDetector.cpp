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

bool
BeatDetector::Detect(MotionState const & state, double & strength)
{
	// Calculate a_t
	//auto prod = ublas::inner_prod(state.acceleration.data(), state.velocity.data());
	//auto a_t = acceleration_t::from_value(prod / geometry::abs(state.velocity).value());

	double vy = state.velocity.get_raw<coord::Y>();
	strength = vyFir_.Run(vy);
	if (peakDetector_.Run(strength) == PeakDetector::Max &&
		strength > 0.5) {
		return true;
	}

	return false;
}

} // namespace MotionTracker
} // namespace cf
