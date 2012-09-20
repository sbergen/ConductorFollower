#include "BeatDetector.h"

#include "cf/math.h"

#include "FrameRateDependent.h"

namespace cf {
namespace MotionTracker {

namespace si = boost::units::si;

BeatDetector::BeatDetector()
{
	ResetBottom();
}

void
BeatDetector::ResetBottom()
{
	bottomPos_.set_raw<coord::Y>(std::numeric_limits<double>::max());
}

bool
BeatDetector::Detect(MotionState const & state, double & strength)
{
	// Calculate a_t
	//auto prod = ublas::inner_prod(state.acceleration.data(), state.velocity.data());
	//auto a_t = acceleration_t::from_value(prod / geometry::abs(state.velocity).value());

	strength = 0.0;

	double vy = state.velocity.get_raw<coord::Y>();
	double vyOut = vyFir_.Run(vy);

	bool bottom = prevVy_ < 0.0 && vyOut >= 0.0;
	prevVy_ = vyOut;

	if (bottom) {
		bottomPos_ = state.position;
	} else {
		Point3D::quantity diff = state.position.get<coord::Y>() - bottomPos_.get<coord::Y>();
		Point3D::quantity threshold(2.0 * si::centi * si::meters);
		if (diff > threshold) {
			strength = 1.0;
			ResetBottom();
		}
	}
	
	return strength > 0.0;
}

} // namespace MotionTracker
} // namespace cf
