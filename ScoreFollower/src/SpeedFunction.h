#pragma once

#include "ScoreFollower/types.h"

#include <boost/utility.hpp>

namespace cf {
namespace ScoreFollower {

class SpeedFunction : public boost::noncopyable
{
public:
	typedef boost::units::quantity<score::speed_change> SpeedChangeRate;

	void SetParameters(speed_t const & reference_speed, real_time_t const & reference_time, SpeedChangeRate const & changePerTimeUnit)
	{
		reference_speed_ = reference_speed;
		reference_time_ = reference_time;
		changePerTimeUnit_ = changePerTimeUnit;
	}

	speed_t NewSpeed(real_time_t const & time)
	{
		auto timeDiff = time::quantity_cast<time_quantity>(time - reference_time_);
		return reference_speed_ + (timeDiff * changePerTimeUnit_);
	}

private:
	speed_t reference_speed_;
	real_time_t reference_time_;
	SpeedChangeRate changePerTimeUnit_;
};

} // namespace ScoreFollower
} // namespace cf
