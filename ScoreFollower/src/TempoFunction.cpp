#include "TempoFunction.h"

#include <cmath>

#include "cf/globals.h"
#include "cf/unit_helpers.h"

#define DEBUG_TEMPO_FUNCTION 1

namespace cf {
namespace ScoreFollower {

namespace bu = boost::units;

const double TempoFunction::pi = std::atan(1.0) * 4.0;

TempoFunction::TempoFunction()
	: startTempo_(0.0 * score::beats_per_second)
	, changeTime_(0.0 * score::seconds)
{}

void
TempoFunction::SetParameters(
	real_time_t const & startTime, time_quantity const & changeTime,
	tempo_t const & startTempo, tempo_t const & tempoChange,
	beat_pos_t const & offset, double offsetCompensationFactor)
{
	// Set variables
	startTime_ = startTime;
	changeTime_ = changeTime;
	startTempo_ = startTempo;
	tempoChange_ = tempoChange;
	startOffset_ = offset;
	offsetCompensationFactor_ = offsetCompensationFactor;

	// Caluculate parameters
	linearTempoChange_ = tempoChange / changeTime;
	auto linearOffset = LinearOffsetAfter(changeTime);

	// non-linear catchup is a * sin(x pi / t),
	// total catchup = 2at/pi, (integral from 0 to t)
	// => a = pi * catchup / 2 t
	auto catchupLeft = (offset * offsetCompensationFactor_) - linearOffset;
	nonLinearCoef_ = pi * catchupLeft.value() / (2 * changeTime.value());

#if DEBUG_TEMPO_FUNCTION
	LOG("Linear offset: %1%", linearOffset);
	LOG("Linear change: %1%, non-linear coef: %2%", linearTempoChange_.value(), nonLinearCoef_);
#endif
}

void
TempoFunction::SetConstantTempo(tempo_t const & tempo)
{
	startTempo_ = tempo;
	changeTime_ = 0.0 * score::seconds;
}

tempo_t
TempoFunction::TempoAt(real_time_t const & time) const
{
	time_quantity t = time_cast<time_quantity>(time - startTime_);
	t = bu::min(t, changeTime_);
	return startTempo_ + LinearTempoChangeAfter(t) + NonLinearTempoChangeAfter(t);
}

beat_pos_t
TempoFunction::OffsetAt(real_time_t const & time) const
{
	time_quantity t = time_cast<time_quantity>(time - startTime_);
	t = bu::min(t, changeTime_);
	return startOffset_ - (LinearOffsetAfter(t) + NonLinearOffsetAfter(t));
}

void
TempoFunction::ScaleToRelativeTempoChange(real_time_t const & time, double coef)
{
	auto const timeLeft = startTime_ + time_cast<duration_t>(changeTime_) - time;
	if (timeLeft.count() <= 0) {
		SetConstantTempo(coef * TempoAt(time));
		return;
	}

	auto changeTime = time_cast<time_quantity>(timeLeft);
	
	auto oldTargetTempo = startTempo_ + tempoChange_;
	auto newTargetTempo = oldTargetTempo * coef;
	auto newTempoNow = TempoAt(time) * coef;
	auto newTempoChange = newTargetTempo - newTempoNow;

	auto newOffsetNow = OffsetAt(time) / coef;

	SetParameters(time, changeTime,
		newTempoNow, newTempoChange,
		newOffsetNow, offsetCompensationFactor_);
}

tempo_t
TempoFunction::LinearTempoChangeAfter(time_quantity const & time) const
{
	auto ret = linearTempoChange_ * time;
	return ret;
}

tempo_t
TempoFunction::NonLinearTempoChangeAfter(time_quantity const & time) const
{
	if (changeTime_.value() == 0.0) { return 0.0 * score::beats_per_second; }

	double t = (time / changeTime_) * pi;
	auto ret = nonLinearCoef_ * std::sin(t) * score::beats_per_second;
	return ret;
}

beat_pos_t
TempoFunction::LinearOffsetAfter(time_quantity const & time) const
{
	// tempo diff to target = linearTempoChange * time - tempoChange
	// integral = 0.5 * (linearTempoChange * t^2) - tempoChange * t

	return unit_integral<score::tempo>(time,
		[this](double t)
		{
			// Integral ax = ax^2/2
			// The cathup is negative, because we are lagging
			//  behind the target tempo at ax
			return (0.5 * linearTempoChange_.value() * t * t) - (tempoChange_.value() * t);
		});
}

beat_pos_t
TempoFunction::NonLinearOffsetAfter(time_quantity const & time) const
{
	if (changeTime_.value() == 0.0) { return 0.0 * score::beats; }

	double t = (time / changeTime_) * pi;
	// Integral of a sin(x pi / t) = -(at / pi) * cos(pi x / t)
	// => normalize to 0 at x = 0:  (at / pi) * (1 - cos(pi x / t))
	// at/pi... something already factored in, so divide per pi
	// TODO clean up the math later!
	return beat_pos_t::from_value(nonLinearCoef_ / pi * (1.0 - std::cos(t)));
}

} // namespace ScoreFollower
} // namespace cf
