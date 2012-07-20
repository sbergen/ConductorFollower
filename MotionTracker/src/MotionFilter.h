#pragma once

#include <boost/array.hpp>

#include "cf/EventBuffer.h"
#include "cf/geometry.h"
#include "cf/time.h"
#include "cf/math.h"
#include "cf/SavitzkyGolay.h"

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
	MotionFilter(InterThreadEventBuffer & eventBuffer);

	void NewPosition(timestamp_t const & time, Point3D const & pos);

private:
	// Run filter from eventBuffer_ and commit events
	void RunFilter(timestamp_t const & time);


	// Helpers for RunFilter()
	void EvaluateCoefs();
	void CommitEvents(timestamp_t const & time);

private:
	// "global" state
	InterThreadEventBuffer & eventBuffer_;
	PositionBuffer positionBuffer_;

	Filter filter_;

	Point3D position_;
	Velocity3D velocity_;
	Acceleration3D acceleration_;
	//Jerk3D jerk_;
};


} // namespace MotionTracker
} // namespace cf
