#include <boost/test/unit_test.hpp>

#include "cf/polynomial.h"

BOOST_AUTO_TEST_SUITE(PolynomialTest)

using namespace cf::math;

// Manual version of the polynomials being tested
double f(double x) { return 2.0 * (x * x) - 5.0 * x + 3.0; }
double f2(double x) { return 3.0 * (x * x) - 5.0 * x + 2.0; }

BOOST_AUTO_TEST_CASE(TestEvaluation)
{
	Vector coefs(3);
	coefs(0) = 3.0;
	coefs(1) = -5.0;
	coefs(2) = 2.0;

	BOOST_CHECK_CLOSE(f(2.0), evaluate_polynomial(coefs, 2.0), 0.01);
	BOOST_CHECK_CLOSE(f(20.0), evaluate_polynomial(coefs, 20.0), 0.01);
	BOOST_CHECK_CLOSE(f(-100.0), evaluate_polynomial(coefs, -100.0), 0.01);
}

BOOST_AUTO_TEST_CASE(TestDerivativeCoefs)
{
	BOOST_CHECK_EQUAL(derivative_coef<2>(1), 0);
	BOOST_CHECK_EQUAL(derivative_coef<2>(2), 2);
	BOOST_CHECK_EQUAL(derivative_coef<2>(3), 6);
	BOOST_CHECK_EQUAL(derivative_coef<2>(4), 12);
	BOOST_CHECK_EQUAL(derivative_coef<2>(5), 20);

	BOOST_CHECK_EQUAL((derivative_coef_c<2, 1>::value), 0);
	BOOST_CHECK_EQUAL((derivative_coef_c<2, 2>::value), 2);
	BOOST_CHECK_EQUAL((derivative_coef_c<2, 3>::value), 6);
	BOOST_CHECK_EQUAL((derivative_coef_c<2, 4>::value), 12);
	BOOST_CHECK_EQUAL((derivative_coef_c<2, 5>::value), 20);
}

BOOST_AUTO_TEST_CASE(TestDerivative)
{
	Vector coefs(3);
	coefs(0) = 3.0;
	coefs(1) = -5.0;
	coefs(2) = 2.0;

	auto d1 = derivative<1>(coefs);
	auto d2 = derivative<2>(coefs);

	BOOST_CHECK_EQUAL(d1.size(), 2);
	BOOST_CHECK_EQUAL(d2.size(), 1);

	BOOST_CHECK_CLOSE(d1(0), -5, 0.01);
	BOOST_CHECK_CLOSE(d1(1), 4.0, 0.01);
	BOOST_CHECK_CLOSE(d2(0), 4.0, 0.01);
}

BOOST_AUTO_TEST_CASE(TestFitting)
{
	Vector x(5), y(5), coefs(3);
	for (int i = 0; i < 5; ++i) {
		x[i] = i;
		y[i] = f(i);
	}

	BOOST_CHECK(fit_polynomial(x, y, coefs));

	BOOST_CHECK_CLOSE(coefs(0), 3.0, 0.01);
	BOOST_CHECK_CLOSE(coefs(1), -5.0, 0.01);
	BOOST_CHECK_CLOSE(coefs(2), 2.0, 0.01);
}

BOOST_AUTO_TEST_CASE(TestMultipleFitting)
{
	Vector x(5), y(5), y2(5);
	for (int i = 0; i < 5; ++i) {
		x[i] = i;
		y[i] = f(i);
		y2[i] = f2(i);
	}

	std::vector<Vector> coefVector(2);

	std::vector<Vector> yVector;
	yVector.push_back(y);
	yVector.push_back(y2);

	BOOST_CHECK(fit_polynomials(2, x, yVector, coefVector));

	BOOST_CHECK_CLOSE(coefVector[0](0), 3.0, 0.01);
	BOOST_CHECK_CLOSE(coefVector[0](1), -5.0, 0.01);
	BOOST_CHECK_CLOSE(coefVector[0](2), 2.0, 0.01);

	BOOST_CHECK_CLOSE(coefVector[1](0), 2.0, 0.01);
	BOOST_CHECK_CLOSE(coefVector[1](1), -5.0, 0.01);
	BOOST_CHECK_CLOSE(coefVector[1](2), 3.0, 0.01);
}

BOOST_AUTO_TEST_SUITE_END()