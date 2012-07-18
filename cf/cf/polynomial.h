#pragma once

#include "cf/math.h"
#include "cf/polynomial_detail.h"

namespace cf {
namespace math {

float_type evaluate_polynomial(Vector const & coefs, float_type x)
{
	if (coefs.size() == 0) { return 0.0; }
	float_type result = coefs(0);
	for (int i = 1; i < coefs.size(); ++i) {
		result += coefs(i) * std::pow(x, i);
	}
}

// Least squares polynomial regression, fits
// y_i = coefs[0] + coefs[1] * x_i + ... + coefs[n] * x_i^(n-1)
// to given x and y data. Number of coefficients (n) is derived from size of coefs.
bool fit_polynomial(Vector const & x, Vector const & y, Vector & coefs)
{
	unsigned const order = coefs.size() - 1;

	Matrix M;
	if (!detail::make_polynomial_fit_matrix(x, order, M)) { return false; }

	coefs = prod(M, y);
	return true;
}

// Optimized version for doing several calculations from the same x values
// C1 and C2 must be containers of vectors
template<typename C1, typename C2>
bool fit_polynomials(unsigned order, Vector const & x, C1 const & yValues, C2 & coefs)
{
	Matrix M;
	if (!detail::make_polynomial_fit_matrix(x, order, M)) { return false; }

	std::transform(std::begin(yValues), std::end(yValues), std::begin(coefs),
		[&M](Vector const & y)
		{
			return prod(M, y);
		});
 
	return true;
}

template<unsigned n>
Vector derivative(Vector const & coefs)
{
	const Vector::size_type size = coefs.size() - n;
    Vector r(size);
    for(Vector::size_type i = 0; i < size; ++i) {
            r(i) = detail::d_coef<n>(i + n) * coefs(i + n);
    }
    return r;
}

	
} // namespace math
} // namespace cf
