#include "BeatDetector.h"

#include "cf/math.h"

#include "FrameRateDependent.h"

namespace cf {
namespace MotionTracker {

namespace si = boost::units::si;

BeatDetector::BeatDetector(MusicalContextBuffer::Reader & musicalContextReader)
	: musicalContextReader_(musicalContextReader)
{
	ResetBottom();
}

void
BeatDetector::ResetBottom()
{
	bottomPos_.set_raw<coord::Y>(std::numeric_limits<double>::max());
	bottomTime_ = timestamp_t::min();
}

bool
BeatDetector::Detect(timestamp_t const & time, MotionState const & state, double & strength)
{
	// Calculate a_t
	//auto prod = ublas::inner_prod(state.acceleration.data(), state.velocity.data());
	//auto a_t = acceleration_t::from_value(prod / geometry::abs(state.velocity).value());

	strength = 0.0;

	double vy = state.fastVelocity.get_raw<coord::Y>();
	bool bottom = prevVy_ < 0.0 && vy >= 0.0;
	prevVy_ = vy;

	if (bottom) {
		bottomTime_ = time;
		bottomPos_ = state.position;
	} else {
		Point3D::quantity yDiff = state.position.get<coord::Y>() - bottomPos_.get<coord::Y>();
		Point3D::quantity yThreshold(2.0 * si::centi * si::meters);
		Point3D::quantity timeResetYThreshold(0.5 * si::centi * si::meters);

		auto timeDiff = time_cast<time_quantity>(time - bottomTime_);
		time_quantity timeThreshold(0.2 * score::beats / musicalContextReader_->currentTempo);

		if (yDiff > yThreshold && timeDiff >= timeThreshold) {
			strength = yDiff / yThreshold;
			ResetBottom();
		} else if (yDiff < timeResetYThreshold) {
			bottomTime_ = time;
		}
	}
	
	return strength > 0.0;
}

} // namespace MotionTracker
} // namespace cf
