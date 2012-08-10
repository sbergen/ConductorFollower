#pragma once

#include "cf/PeakHolder.h"

namespace cf {

template<unsigned Length, typename T = double>
class DynamicRange
{
public:
	T Run(T const & newVal)
	{
		auto peak = peakHolder_.Run(newVal);
		auto dip = dipHolder_.Run(newVal);
		return peak - dip;
	}

private:
	PeakHolder<Length, T> peakHolder_;
	DipHolder<Length, T> dipHolder_;
};


} // namespace cf
