#pragma once

#include <boost/array.hpp>

#include "cf/EventBuffer.h"
#include "cf/physics.h"
#include "cf/time.h"
#include "cf/math.h"
#include "cf/SavitzkyGolay.h"

#include "MotionTracker/MotionState.h"

#include "common_types.h"
#include "FrameRateDependent.h"

namespace cf {
namespace MotionTracker {

class MotionFilter
{
private:
	typedef EventBuffer<Point3D, timestamp_t> PositionBuffer;
	typedef SmoothingSavitzkyGolay<FrameRateDependent::filter_size, FrameRateDependent::filter_order, 3> Filter;

public:
	MotionFilter();

	bool NewPosition(timestamp_t const & time, Point3D const & pos);
	MotionState const & State() const { return motionState_; }

private:
	// Run filter from eventBuffer_ and commit events
	void RunFilter();
	void EvaluateCoefs();

private:
	Filter filter_;
	MotionState motionState_;
};


} // namespace MotionTracker
} // namespace cf
