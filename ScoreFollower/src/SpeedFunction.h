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

	speed_t NewSpeed(real_time_t const & time)
	{
		auto timeDiff = time::quantity_cast<time_quantity>(time - reference_time_);
		timeDiff = boost::units::min(timeDiff, accelerationTime_);
		return reference_speed_ + (timeDiff * changePerTimeUnit_);
	}

private:
	speed_t reference_speed_;
	real_time_t reference_time_;
	SpeedChangeRate changePerTimeUnit_;
	time_quantity accelerationTime_;
};

} // namespace ScoreFollower
} // namespace cf
