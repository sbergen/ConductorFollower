#include "StartGestureDetector.h"

#include "cf/globals.h"

namespace cf {
namespace MotionTracker {

namespace si = boost::units::si;

StartGestureDetector::StartGestureDetector(MusicalContextBuffer::Reader & musicalContextReader)
	: musicalContextReader_(musicalContextReader)
	, previousSteadyTimeStart_(timestamp_t::min())
	, sufficientSteadyPeriodEnd_(timestamp_t::min())
	, inSteadyState_(false)
	, previousBeatTime_(timestamp_t::min())
	, previousBottomTime_(timestamp_t::min())
{
}

StartGestureDetector::Result
StartGestureDetector::Detect(timestamp_t const & timestamp, MotionState const & state, bool beatOccurred)
{
	if (musicalContextReader_->rolling) { return Result(); }

	// Register beats
	if (beatOccurred) {
		previousBeatTime_ = timestamp;
		previousBeatPosition_ = state.position;
	}

	auto vy = state.fastVelocity.get_raw<coord::Y>();

	// Check steady state
	bool steadyOk = CheckSteadyState(timestamp, vy);

	// check for apex and bottom
	bool bottomOccurred = (prevVy_ < 0.0) && (vy >= 0.0);
	bool apexOccurred = (prevVy_ > 0.0) && (vy <= 0.0);
	prevVy_ = vy;

	if (bottomOccurred)
	{
		previousBottomPosition_ = state.position;
		previousBottomTime_ = timestamp;
	}

	// Check that all events have occurred in the right order
	//LOG("timeDiff: %1%, aped: %2%, steady: %3%", previousBottomTime_ < previousBeatTime_, apexOccurred, steadyOk);
	if (!(previousBottomTime_ < previousBeatTime_ && apexOccurred && steadyOk)) {
		return Result();
	}

	// Check y-movement magnitude
	auto magnitude = geometry::distance_vector(previousBottomPosition_, state.position);
	LOG("Start gesture y magnitude: %1%", magnitude.get<coord::Y>().value());
	if (magnitude.get<coord::Y>() < coord_t(5 * si::centi * si::meters)) { return Result(); }

	// Check duration, the duration is from bottom to apex, which is about half a beat
	duration_t gestureLength = timestamp - previousBottomTime_;
	seconds_t minTempo(60.0 / 40 / 2);
	seconds_t maxTempo(60.0 / 200 / 2);
	LOG("Start gesture length: %1%", gestureLength);
	if (gestureLength > minTempo || gestureLength < maxTempo) { return Result(); }

	// Done!
	return Result(previousBeatTime_, gestureLength);
}

bool
StartGestureDetector::CheckSteadyState(timestamp_t const & timestamp, double yFirOutput)
{
	// Check movement
	bool endedPeriod = false;
	double movementLimit = 1.0e-4;
	if (std::abs(yFirOutput) < movementLimit) {
		if (!inSteadyState_) {
			previousSteadyTimeStart_ = timestamp;
		}

		inSteadyState_ = true;
	} else {
		if (inSteadyState_) {
			endedPeriod = true;
		}

		inSteadyState_ = false;
	}

	if (endedPeriod) {
		// Check duration
		auto durationLimit = milliseconds_t(100);
		auto steadyDuration = timestamp - previousSteadyTimeStart_;
		if (steadyDuration >= durationLimit) {
			sufficientSteadyPeriodEnd_ = timestamp;
		}
	}

	if (sufficientSteadyPeriodEnd_ == timestamp_t::min()) { return false; }

	// Check age
	auto oldLimit = milliseconds_t(1500);
	auto steadyAge = timestamp - sufficientSteadyPeriodEnd_;
	if (steadyAge > oldLimit) { return false; }

	return true;
}

} // namespace MotionTracker
} // namespace cf
