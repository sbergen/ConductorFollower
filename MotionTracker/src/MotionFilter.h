#pragma once

#include <boost/array.hpp>
#include <boost/mpl/int.hpp>

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
	typedef SmoothingSavitzkyGolay<FrameRateDependent::filter_size, FrameRateDependent::filter_order, 3> Filter;
	typedef SmoothingSavitzkyGolay<FrameRateDependent::fast_filter_size, FrameRateDependent::fast_filter_order, 3> FastFilter;

	// TODO is using templates stupid/limiting here? TMP could be used...
	typedef InterpolatingSavitzkyGolay<FrameRateDependent::filter_size, 1, 3> Interpolator1;
	typedef InterpolatingSavitzkyGolay<FrameRateDependent::filter_size, 2, 3> Interpolator2;
	typedef InterpolatingSavitzkyGolay<FrameRateDependent::filter_size, 3, 3> Interpolator3;

	typedef EventBuffer<Point3D, timestamp_t> PositionBuffer;
	typedef EventBuffer<MotionState, timestamp_t> StateCache;

public:
	MotionFilter();

	// New position from sensor, might cause multiple new positions (interpolation)
	// Callback is void(timestamp_t const & time, MotionState const & motionState)
	template<typename Callback>
	void NewPosition(timestamp_t const & time, Point3D const & pos, Callback & callback)
	{
		FillCacheFromNewPosition(time, pos);
		stateCache_.AllEvents().ForEach(callback);
	}

private:
	unsigned EstimateMissedUpdates(timestamp_t const & time);
	void FillCacheFromNewPosition(timestamp_t const & time, Point3D const & pos);
	void RegisterPosition(timestamp_t const & time, Point3D const & pos);
	// Run filter from eventBuffer_ and commit events
	void EvaluateCoefs(MotionState & state);

	template<typename Interpolator>
	void RunInterpolator(Interpolator & interpolator, timestamp_t const & time, Point3D const & pos);

	template<unsigned Offset, typename Interpolator>
	void RegisterInterpolatorResult(boost::mpl::int_<Offset> offset, Interpolator & interpolator, timestamp_t const & time);

	template<typename Interpolator>
	void RegisterInterpolatorResult(boost::mpl::int_<0> offset, Interpolator & interpolator, timestamp_t const & time)
	{ /* Recursion terminator */ }

private:
	time_quantity timeStep_;
	
	Filter filter_;
	FastFilter fastFilter_;

	Interpolator1 interpolator1_;
	Interpolator2 interpolator2_;
	Interpolator3 interpolator3_;

	timestamp_t lastReceivedPosition_;
	PositionBuffer history_;
	StateCache stateCache_;
};


} // namespace MotionTracker
} // namespace cf
