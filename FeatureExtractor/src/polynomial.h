#pragma once

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

//#include "types.h"
#include "inverse_matrix.h"

namespace cf {
namespace FeatureExtractor {
namespace math {

namespace ublas = boost::numeric::ublas;

typedef double float_type;
typedef ublas::vector<float_type> Vector;
typedef ublas::matrix<float_type> Matrix;

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
	unsigned const n = std::min(x.size(), y.size());

	// Make matrix
	Matrix X(n, order + 1);
	for (int i = 0; i < n; ++i) {
		X(i, 0) = 1;
		for (int j = 1; j < order + 1; ++j) {
			X(i, j) = std::pow(x[i], j);
		}
	}

	using ublas::prod;
	using ublas::trans;

	auto Xt = trans(X);
	Matrix XtX = prod(Xt, X);
	Matrix XtXi(XtX.size1(), XtX.size2());
	if (!inverse_matrix(XtX, XtXi)) { return false; }

	// coefs = (Xt X)^-1 Xt y
	coefs = prod(prod(XtXi, Xt), y);
	return true;
}
	
} // namespace math
} // namespace FeatureExtractor
} // namespace cf
