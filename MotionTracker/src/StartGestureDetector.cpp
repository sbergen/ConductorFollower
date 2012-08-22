#include "StartGestureDetector.h"

#include "cf/globals.h"

namespace cf {
namespace MotionTracker {

namespace si = boost::units::si;

StartGestureDetector::StartGestureDetector()
	: previousBeatTime_(timestamp_t::min())
	, previousBottomTime_(timestamp_t::min())
{
}

StartGestureDetector::Result
StartGestureDetector::Detect(timestamp_t const & timestamp, MotionState const & state, bool beatOccurred)
{
	// Register beats
	if (beatOccurred) {
		previousBeatTime_ = timestamp;
		previousBeatPosition_ = state.position;
	}

	// Run velocity Fir and check for apex and bottom
	auto firOutput = velocityFir_.Run(state.velocity.get_raw<coord::Y>());
	bool bottomOccurred = (prevVelocityFirOutput_ < 0.0) && (firOutput >= 0.0);
	bool apexOccurred = (prevVelocityFirOutput_ > 0.0) && (firOutput <= 0.0);
	prevVelocityFirOutput_ = firOutput;

	if (bottomOccurred)
	{
		previousBottomPosition_ = state.position;
		previousBottomTime_ = timestamp;
	}

	// Check that all three events have occurred in the right order
	if (!(previousBottomTime_ < previousBeatTime_ && apexOccurred)) {
		return Result();
	}

	// Check y-movement magnitude
	auto magnitude = geometry::distance_vector(previousBottomPosition_, state.position);
	LOG("Start gesture y magnitude: %1%", magnitude.get<coord::Y>().value());
	if (magnitude.get<coord::Y>() < coord_t(15 * si::centi * si::meters)) { return Result(); }

	// Check duration, the duration is from bottom to apex, which is about half a beat
	duration_t gestureLength = timestamp - previousBottomTime_;
	seconds_t minTempo(60.0 / 40 / 2);
	seconds_t maxTempo(60.0 / 200 / 2);
	LOG("Start gesture length: %1%", gestureLength);
	if (gestureLength > minTempo || gestureLength < maxTempo) { return Result(); }

	// Done!
	return Result(previousBeatTime_);
}

} // namespace MotionTracker
} // namespace cf
