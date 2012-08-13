#include <boost/test/unit_test.hpp>

#include <boost/numeric/ublas/io.hpp>

#include "cf/SavitzkyGolay.h"
#include "cf/polynomial.h"

BOOST_AUTO_TEST_SUITE(SavitzxkyGolayTests)

using namespace cf;
using namespace cf::math;

BOOST_AUTO_TEST_CASE(TestFilter)
{
	SmoothingSavitzkyGolay<5, 3, 1> filter(0.5);

	// Reference polynomial
	Vector p(4);
	p(0) = 6.0;
	p(1) = 3.0;
	p(2) = -2.0;
	p(3) = 1.0;

	Vector x(1);
	x(0) = evaluate_polynomial(p, -1.0); filter.AppendValue(x);
	x(0) = evaluate_polynomial(p, -0.5); filter.AppendValue(x);
	x(0) = evaluate_polynomial(p,  0.0); filter.AppendValue(x);
	x(0) = evaluate_polynomial(p,  0.5); filter.AppendValue(x);
	x(0) = evaluate_polynomial(p,  1.0); filter.AppendValue(x);

	filter.RunFromValues();

	Vector y(1);

	filter.EvaluateDerivative<0>(y);
	BOOST_CHECK_CLOSE(evaluate_polynomial(p, 0.0), y(0), 0.01);

	filter.EvaluateDerivative<1>(y);
	BOOST_CHECK_CLOSE(evaluate_polynomial(derivative<1>(p), 0.0), y(0), 0.01);

	filter.EvaluateDerivative<2>(y);
	BOOST_CHECK_CLOSE(evaluate_polynomial(derivative<2>(p), 0.0), y(0), 0.01);

	filter.EvaluateDerivative<3>(y);
	BOOST_CHECK_CLOSE(evaluate_polynomial(derivative<3>(p), 0.0), y(0), 0.01);
}

BOOST_AUTO_TEST_CASE(TestInterpolation)
{
	// Filter definition, length 7, skip 2, one dimensional
	math::float_type xStep = 0.03333333;
	InterpolatingSavitzkyGolay<7, 2, 1> filter(xStep);

	// Reference polynomial, max order 4, for length of 7 and skip of 2 (7 - 2 - 1 = 4)
	Vector p(4);
	p(0) = 7.0;
	p(1) = 3.0;
	p(2) = -5.0;
	p(3) = 2.0;

	// We should have 5 values in the estimation vector (7 - 2 = 5)
	boost::array<Vector, 5> x;
	for (int i = 0; i < 5; ++i) {
		x[i].resize(1);
	}
	x[0][0] = evaluate_polynomial(p, 0 * xStep);
	x[1][0] = evaluate_polynomial(p, 1 * xStep);
	x[2][0] = evaluate_polynomial(p, 2 * xStep);
	x[3][0] = evaluate_polynomial(p, 3 * xStep);
	// and the...                    4
	// missing values                5
	x[4][0] = evaluate_polynomial(p, 6 * xStep);

	// Now run filter
	filter.RunFromValues(x);

	// And check the restults
	Vector y(1); // result vector

	filter.NthMissingValue<0>(y);
	BOOST_CHECK_CLOSE(evaluate_polynomial(p, 4 * xStep), y(0), 0.01);

	filter.NthMissingValue<1>(y);
	BOOST_CHECK_CLOSE(evaluate_polynomial(p, 5 * xStep), y(0), 0.01);
}

BOOST_AUTO_TEST_SUITE_END()