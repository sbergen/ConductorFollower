#include "MotionFilter.h"

#include "cf/globals.h"
#include "cf/polynomial.h"
#include "cf/LinearRationalInterpolator.h"

namespace cf {
namespace MotionTracker {

namespace si = boost::units::si;
namespace mpl = boost::mpl;

MotionFilter::MotionFilter()
	: timeStep_((1.0 / FrameRateDependent::frame_rate) * si::seconds) 
	, filter_(timeStep_.value())
	, interpolator1_(timeStep_.value())
	, interpolator2_(timeStep_.value())
	, interpolator3_(timeStep_.value())
	, lastReceivedPosition_(timestamp_t::min())
	// Full filter worth of history
	, history_(FrameRateDependent::filter_size)
	// Max 1 new state + 3 interpolations
	, stateCache_(4)
{
}

unsigned
MotionFilter::EstimateMissedUpdates(timestamp_t const & time)
{
	auto timeSinceLast = time::quantity_cast<time_quantity>(
		time - lastReceivedPosition_);
	auto estimate = timeSinceLast / timeStep_;
	unsigned intEstimate = static_cast<unsigned>(math::round(estimate));
	return (intEstimate == 0) ? 0 : intEstimate - 1;
}

void
MotionFilter::FillCacheFromNewPosition(timestamp_t const & time, Point3D const & pos)
{
	static int positionsReceived = 0;
	stateCache_.Clear();

	// We need to fill the filter straight out once, before we can trust anything
	// (we could probably do better, but it's not worth it...)
	if (positionsReceived < FrameRateDependent::filter_size) {
		if (EstimateMissedUpdates(time) > 0) {
			positionsReceived = 0;
		}
		++positionsReceived;

		lastReceivedPosition_ = time;
		history_.RegisterEvent(time, pos);
		filter_.AppendValue(pos.data());
		return;
	}

	// Now we are guaranteed to have a full filter worth of data,
	// and we can go the regular route. Note that the if above was not executed!
	
	// first check need for interpolation and register the interpolated points
	auto const missedUpdated = EstimateMissedUpdates(time);
	switch(missedUpdated)
	{
	case 0:
		break;
	case 1:
		RunInterpolator(interpolator1_, time, pos);
		break;
	case 2:
		RunInterpolator(interpolator2_, time, pos);
		break;
	case 3:
		RunInterpolator(interpolator3_, time, pos);
		break;
	default:
		// TODO fail in a different way?
		positionsReceived = 0;
		return;
	}

	if (missedUpdated > 0) {
		LOG("--- Missed %1% samples", EstimateMissedUpdates(time));
	}

	// ...and the "real" one
	lastReceivedPosition_ = time;
	RegisterPosition(time, pos);
}

void
MotionFilter::RegisterPosition(timestamp_t const & time, Point3D const & pos)
{	
	history_.RegisterEvent(time, pos);
	filter_.AppendValue(pos.data());

	MotionState state;
	filter_.RunFromValues();
	EvaluateCoefs(state);
	stateCache_.RegisterEvent(time, state);
}

void
MotionFilter::EvaluateCoefs(MotionState & state)
{
	filter_.EvaluateDerivative<0>(state.position.data());
	filter_.EvaluateDerivative<1>(state.velocity.data());
	filter_.EvaluateDerivative<2>(state.acceleration.data());
	filter_.EvaluateDerivative<3>(state.jerk.data());
}

template<typename Interpolator>
void
MotionFilter::RunInterpolator(Interpolator & interpolator, timestamp_t const & time, Point3D const & pos)
{
	// Init stuff
	boost::array<Point3D::data_type, Interpolator::matrix_size> data;
	auto const lastDataIndex = Interpolator::matrix_size - 1;
	auto history = history_.LastNumEvnets(lastDataIndex);

	// Fill data
	for(int i = 0; i < lastDataIndex; ++i) {
		data[i] = history[i].data.data();
	}
	data[lastDataIndex] = pos.data();

	// Run and register
	interpolator.RunFromValues(data);
	RegisterInterpolatorResult(mpl::int_<Interpolator::skip_amount>(), interpolator, time);
}

template<unsigned Offset, typename Interpolator>
void
MotionFilter::RegisterInterpolatorResult(boost::mpl::int_<Offset> offset, Interpolator & interpolator, timestamp_t const & time)
{
	// Register results
	timestamp_t iTime;
	Point3D::data_type iPos(Point3D::dimension);

	auto quantity = math::LinearRationalInterpolator<1, Interpolator::skip_amount + 1>(
		time::quantity_cast<time_quantity>(lastReceivedPosition_.time_since_epoch()),
		time::quantity_cast<time_quantity>(time.time_since_epoch()));
	iTime = timestamp_t::time_point(time::duration_cast<duration_t>(quantity));
	interpolator.NthMissingValue<Interpolator::skip_amount - Offset>(iPos);
	RegisterPosition(iTime, iPos);

	// Recurse
	RegisterInterpolatorResult(mpl::int_<Offset - 1>(), interpolator, time);
}

} // namespace MotionTracker
} // namespace cf
