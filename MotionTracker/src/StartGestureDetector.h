#pragma once

#include "cf/Fir.h"

#include "MotionTracker/MotionState.h"

namespace cf {
namespace MotionTracker {

class StartGestureDetector
{
public:
	StartGestureDetector();

	struct Result
	{
		Result() : previousBeatTime(timestamp_t::min()) {}
		Result(timestamp_t const & prevBeat) : previousBeatTime(prevBeat) {}

		operator bool() const { return previousBeatTime != timestamp_t::min(); }
		timestamp_t previousBeatTime;
	};

	Result Detect(timestamp_t const & timestamp, MotionState const & state, bool beatOccurred);

private:
	AveragingFir<3> velocityFir_;
	double prevVelocityFirOutput_;

	timestamp_t previousBeatTime_;
	Point3D previousBeatPosition_;
};


} // namespace MotionTracker
} // namespace cf
