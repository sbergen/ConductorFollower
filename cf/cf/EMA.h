#pragma once

#include <numeric>

#include "cf/math.h"

namespace cf {

// Exponential Moving Average
template<unsigned Lag, typename T = double>
class EMA
{
public:
	EMA()
		: coef_(static_cast<math::float_type>(1.0) / (1 + Lag))
		, coefComplement_(static_cast<math::float_type>(1.0) - coef_)
		, previousValue_()
	{}

	T Run(T const & nextValue)
	{
		previousValue_ = coef_ * nextValue +
		                 coefComplement_ * previousValue_;
		return previousValue_;
	}

private:
	math::float_type const coef_;
	math::float_type const coefComplement_;
	T previousValue_;
};

} // namespace cf
