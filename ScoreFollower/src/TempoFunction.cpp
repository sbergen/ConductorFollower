#include "TempoFunction.h"

#include <cmath>

#include "cf/unit_helpers.h"

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
	beat_pos_t const & catchup)
{
	// Set variables
	startTime_ = startTime;
	changeTime_ = changeTime;
	startTempo_ = startTempo_;

	// Caluculate parameters
	linearTempoChange_ = tempoChange / changeTime;
	auto linearCatchup = LinearCatchupAfter(changeTime);

	// non-linear catchup is a * sin(t),
	// t is scaled to 0 -> 0, changeTime -> pi
	// total catchup is 2a,
	// => a = catchup / 2
	auto catchupLeft = catchup - linearCatchup;
	nonLinearCoef_ = 0.5 * catchupLeft.value();
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
	time_quantity t = time::quantity_cast<time_quantity>(time - startTime_);
	t = bu::min(t, changeTime_);
	return startTempo_ + LinearTempoChangeAfter(t) + NonLinearTempoChangeAfter(t);
}

beat_pos_t
TempoFunction::CatchupAt(real_time_t const & time) const
{
	time_quantity t = time::quantity_cast<time_quantity>(time - startTime_);
	t = bu::min(t, changeTime_);
	return LinearCatchupAfter(t) + NonLinearCatchupAfter(t);
}

tempo_t
TempoFunction::LinearTempoChangeAfter(time_quantity const & time) const
{
	return linearTempoChange_ * time;
}

tempo_t
TempoFunction::NonLinearTempoChangeAfter(time_quantity const & time) const
{
	if (changeTime_.value() == 0.0) { return 0.0 * score::beats_per_second; }

	double t = time / changeTime_;
	return nonLinearCoef_ * std::sin(t) * score::beats_per_second;
}

beat_pos_t
TempoFunction::LinearCatchupAfter(time_quantity const & time) const
{
	return unit_integral<score::tempo>(time,
		[this](double t)
		{
			// Integral ax = ax^2/2
			return 0.5 * linearTempoChange_.value() * t * t;
		});
}

beat_pos_t
TempoFunction::NonLinearCatchupAfter(time_quantity const & time) const
{
	return unit_integral<score::tempo>(time,
		[this](double t)
		{
			// Integral of a sin(x) = -a cos(x)
			// => normalize to 0 at x: a (1 - cos(x))
			return nonLinearCoef_ * (1.0 - std::cos(t));
		});
}

} // namespace ScoreFollower
} // namespace cf
