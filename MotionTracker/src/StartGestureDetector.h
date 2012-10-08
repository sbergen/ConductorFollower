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
		Result(timestamp_t const & prevBeat, duration_t const & gestureLength)
			: previousBeatTime(prevBeat)
			, gestureLength(gestureLength)
		{}

		operator bool() const { return previousBeatTime != timestamp_t::min(); }
		timestamp_t previousBeatTime;
		duration_t gestureLength;
	};

	Result Detect(timestamp_t const & timestamp, MotionState const & state, bool beatOccurred);

private:
	bool CheckSteadyState(timestamp_t const & timestamp, double yFirOutput);

private:
	double prevVy_;

	timestamp_t previousSteadyTimeStart_;
	bool inSteadyState_;
	timestamp_t sufficientSteadyPeriodEnd_;

	timestamp_t previousBeatTime_;
	Point3D previousBeatPosition_;

	timestamp_t previousBottomTime_;
	Point3D previousBottomPosition_;
};


} // namespace MotionTracker
} // namespace cf
