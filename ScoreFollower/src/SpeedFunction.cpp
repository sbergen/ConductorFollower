#include "SpeedFunction.h"

namespace cf {
namespace ScoreFollower {

SpeedFunction::SpeedFunction()
	: reference_speed_(1.0)
	, changePerTimeUnit_(0.0 * score::fractions_per_second)
	, accelerationTime_(0.0 * score::seconds)
{}

void
SpeedFunction::SetParameters(speed_t const & reference_speed, real_time_t const & reference_time,
	SpeedChangeRate const & changePerTimeUnit, time_quantity const & accelerationTime)
{
	reference_speed_ = reference_speed;
	reference_time_ = reference_time;
	changePerTimeUnit_ = changePerTimeUnit;
	accelerationTime_ = accelerationTime;
}

void
SpeedFunction::SetConstantSpeed(speed_t const & speed)
{
	SetParameters(
		speed,
		real_time_t::min(),
		0.0 * score::fractions_per_second,
		0.0 * score::seconds);
}

speed_t
SpeedFunction::SpeedAt(real_time_t const & time) const
{
	// The integral of sqrt(x) from 0 to 1 is 4/3 times the integral
	// of x from 0 to 1, so we need to scale this here
	double fraction = 0.75 * FractionAt(time);
	return reference_speed_ + (fraction * accelerationTime_ * changePerTimeUnit_);
}

double
SpeedFunction::FractionAt(real_time_t const & time) const
{
	namespace bu = boost::units;

	if (accelerationTime_ == 0.0 * bu::si::seconds) { return 1.0; }

	auto timeDiff = time::quantity_cast<time_quantity>(time - reference_time_);
	timeDiff = bu::min(timeDiff, accelerationTime_);
	return std::pow(timeDiff / accelerationTime_, 0.5);
}


} // namespace ScoreFollower
} // namespace cf
