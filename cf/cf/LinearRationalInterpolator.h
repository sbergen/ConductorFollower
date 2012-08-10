#pragma once

#include <boost/static_assert.hpp>

#include "cf/math.h"

namespace cf {
namespace math {

template<unsigned N, unsigned Division, typename T>
T LinearRationalInterpolator(T const & min, T const & max)
{
	BOOST_STATIC_ASSERT(N < Division);
	return min + ((static_cast<float_type>(N) / Division) * (max - min));
}

} // namespace math
} // namespace cf
