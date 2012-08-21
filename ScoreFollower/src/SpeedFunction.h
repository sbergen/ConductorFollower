#pragma once

#include "cf/units_math.h"

#include "ScoreFollower/types.h"

#include <boost/utility.hpp>

namespace cf {
namespace ScoreFollower {

class SpeedFunction : public boost::noncopyable
{
public:
	typedef boost::units::quantity<score::speed_change> SpeedChangeRate;

	SpeedFunction()
		: reference_speed_(1.0)
		, changePerTimeUnit_(0.0 * score::fractions_per_second)
		, accelerationTime_(0.0 * score::seconds)
	{}

	void SetParameters(speed_t const & reference_speed, real_time_t const & reference_time,
		SpeedChangeRate const & changePerTimeUnit, time_quantity const & accelerationTime)
	{
		reference_speed_ = reference_speed;
		reference_time_ = reference_time;
		changePerTimeUnit_ = changePerTimeUnit;
		accelerationTime_ = accelerationTime;
	}

	void SetConstantSpeed(speed_t const & speed)
	{
		SetParameters(
			speed,
			real_time_t::min(),
			0.0 * score::fractions_per_second,
			0.0 * score::seconds);
	}

	speed_t SpeedAt(real_time_t const & time) const
	{
		double fraction = FractionAt(time);
		return reference_speed_ + (fraction * accelerationTime_ * changePerTimeUnit_);
	}

	double FractionAt(real_time_t const & time) const
	{
		namespace bu = boost::units;

		if (accelerationTime_ == 0.0 * bu::si::seconds) { return 1.0; }

		auto timeDiff = time::quantity_cast<time_quantity>(time - reference_time_);
		timeDiff = bu::min(timeDiff, accelerationTime_);
		return std::pow(timeDiff / accelerationTime_, 0.5);
	}

private:
	speed_t reference_speed_;
	real_time_t reference_time_;
	SpeedChangeRate changePerTimeUnit_;
	time_quantity accelerationTime_;
};

} // namespace ScoreFollower
} // namespace cf
