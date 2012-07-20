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

BOOST_AUTO_TEST_SUITE_END()