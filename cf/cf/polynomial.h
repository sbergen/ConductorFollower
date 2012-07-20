#pragma once

#include "cf/math.h"
#include "cf/inverse_matrix.h"

namespace cf {
namespace math {

// coefs are from 0th to Nth degree
inline float_type evaluate_polynomial(Vector const & coefs, float_type x)
{
	if (coefs.size() == 0) { return 0.0; }
	float_type result = coefs(0);
	for (int i = 1; i < coefs.size(); ++i) {
		result += coefs(i) * std::pow(x, i);
	}
	return result;
}

// Derivative coefficients
template<unsigned deg>
unsigned derivative_coef(unsigned exponent)
{
        return exponent * derivative_coef<deg - 1>(exponent - 1);
}
template<>
inline unsigned derivative_coef<1>(unsigned exponent)
{
        return exponent;
}
template<>
inline unsigned derivative_coef<0>(unsigned exponent)
{
        return 1;
}

template<unsigned deg, unsigned exponent>
struct derivative_coef_c
{
	enum { value = exponent * derivative_coef_c<deg - 1, exponent - 1>::value };
};
template<unsigned exponent>
struct derivative_coef_c<1, exponent>
{
	enum { value = exponent };
};
template<unsigned exponent>
struct derivative_coef_c<0, exponent>
{
	enum { value = 1 };
};

// Makes the (Xt X)^-1 Xt part of solving a polynomial fitting problem
inline bool make_polynomial_fit_matrix(Vector const & x, math::uint_type order, Matrix & result)
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

// Least squares polynomial regression, fits
// y_i = coefs[0] + coefs[1] * x_i + ... + coefs[n] * x_i^(n-1)
// to given x and y data.
// Uses precomputed matrix
// Number of coefficients (n) is derived from given matrix
inline void fit_polynomial(Matrix const & matrix, Vector const & y, Vector & coefs)
{
	coefs = prod(matrix, y);
}

// Number of coefficients (n) is derived from size of coefs.
inline bool fit_polynomial(Vector const & x, Vector const & y, Vector & coefs)
{
	unsigned const order = coefs.size() - 1;

	Matrix M;
	if (!make_polynomial_fit_matrix(x, order, M)) { return false; }

	fit_polynomial(M, y, coefs);
	return true;
}

// Optimized version for doing several calculations from the same x values
// C1 and C2 must be containers of vectors
template<typename C1, typename C2>
void fit_polynomials(Matrix const & matrix, C1 const & yValues, C2 & coefs)
{
	std::transform(std::begin(yValues), std::end(yValues), std::begin(coefs),
		[&matrix](Vector const & y)
		{
			return prod(matrix, y);
		});
}

// Same as above, without precalculated matrix
template<typename C1, typename C2>
bool fit_polynomials(unsigned order, Vector const & x, C1 const & yValues, C2 & coefs)
{
	Matrix M;
	if (!make_polynomial_fit_matrix(x, order, M)) { return false; }
	fit_polynomials(M, yValues, coefs);
	return true;
}

template<unsigned n>
Vector derivative(Vector const & coefs)
{
	const Vector::size_type size = coefs.size() - n;
	Vector r(size);
	for(Vector::size_type i = 0; i < size; ++i) {
		r(i) = derivative_coef<n>(i + n) * coefs(i + n);
	}
	return r;
}

	
} // namespace math
} // namespace cf
