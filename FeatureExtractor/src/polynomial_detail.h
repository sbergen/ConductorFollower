#pragma once

#include "inverse_matrix.h"
#include "fe_math.h"

namespace cf {
namespace FeatureExtractor {
namespace math {
namespace detail {

template<unsigned deg>
unsigned d_coef(unsigned exponent)
{
        return exponent * d_coef<deg - 1>(exponent - 1);
}
 
template<>
unsigned d_coef<1>(unsigned exponent)
{
        return exponent;
}

// Makes the (Xt X)^-1 Xt part of solving a polynomial fitting problem
bool make_polynomial_fit_matrix(Vector const & x, math::uint_type order, Matrix & result)
{
	math::uint_type const n = x.size();

	// Make matrix
	Matrix X(n, order + 1);
	for (math::uint_type i = 0; i < n; ++i) {
		X(i, 0) = 1;
		for (math::uint_type j = 1; j < order + 1; ++j) {
			X(i, j) = std::pow(x[i], static_cast<int>(j));
		}
	}

	using ublas::prod;
	using ublas::trans;

	auto Xt = trans(X);
	Matrix XtX = prod(Xt, X);
	Matrix XtXi(XtX.size1(), XtX.size2());
	if (!inverse_matrix(XtX, XtXi)) { return false; }

	// (Xt X)^-1 Xt
	result = prod(XtXi, Xt);
	return true;
}

} // namespace detail
} // namespace math
} // namespace FeatureExtractor
} // namespace cf
