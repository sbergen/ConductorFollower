#pragma once

#include <vector>

#include <boost/array.hpp>

#include "cf/math.h"
#include "types.h"

namespace cf {
namespace FeatureExtractor {

struct PolynomialResult
{
	PolynomialResult() : success(false) {}
	PolynomialResult(boost::array<math::Vector, 3> const & coefs, math::float_type x);

	operator bool() const { return success; }

	bool success;
	Point3D interpolatedPoint;
	Velocity3D instantaneousVelocity;
	Acceleration3D instantaneousAcceleration;
};

PolynomialResult polynomial_analysis(PositionBuffer & buffer);

} // namespace FeatureExtractor
} // namespace cf
