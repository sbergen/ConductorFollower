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

	SpeedFunction();

	void SetParameters(speed_t const & reference_speed, real_time_t const & reference_time,
		SpeedChangeRate const & changePerTimeUnit, time_quantity const & accelerationTime);

	void SetConstantSpeed(speed_t const & speed);
	speed_t SpeedAt(real_time_t const & time) const;
	double FractionAt(real_time_t const & time) const;

private:
	speed_t reference_speed_;
	real_time_t reference_time_;
	SpeedChangeRate changePerTimeUnit_;
	time_quantity accelerationTime_;
};

} // namespace ScoreFollower
} // namespace cf
