#include "StartGestureDetector.h"

#include "cf/globals.h"

namespace cf {
namespace MotionTracker {

namespace si = boost::units::si;

StartGestureDetector::StartGestureDetector()
	: previousBeatTime_(timestamp_t::min())
{
}

StartGestureDetector::Result
StartGestureDetector::Detect(timestamp_t const & timestamp, MotionState const & state, bool beatOccurred)
{
	if (beatOccurred) {
		previousBeatTime_ = timestamp;
		previousBeatPosition_ = state.position;
	}

	if (previousBeatTime_ == timestamp_t::min()) { return Result(); }

	// Run velocity Fir and check for apex
	auto firOutput = velocityFir_.Run(state.velocity.get_raw<coord::Y>());
	bool apexOccurred = (prevVelocityFirOutput_ > 0.0) && (firOutput <= 0.0);
	prevVelocityFirOutput_ = firOutput;
	if (!apexOccurred) { return Result(); }

	// Check y-movement magnitude
	// The beat position is already quite high, we don't need much above that for a proper start
	auto magnitude = geometry::distance_vector(previousBeatPosition_, state.position);
	if (magnitude.get<coord::Y>() < coord_t(4 * si::centi * si::meters)) { return Result(); }

	// Check duration, the duration is from beat to apex, which is about a fourth of a beat
	duration_t gestureLength = timestamp - previousBeatTime_;
	seconds_t minTempo(60.0 / 40 / 4);
	seconds_t maxTempo(60.0 / 200 / 4);
	if (gestureLength > minTempo || gestureLength < maxTempo) { return Result(); }

	// Done!
	return Result(previousBeatTime_);
}

} // namespace MotionTracker
} // namespace cf
