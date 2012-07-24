#pragma once

#include <boost/array.hpp>
#include <boost/static_assert.hpp>

#include "cf/math.h"
#include "cf/polynomial.h"

namespace cf {

// Base class, not useful by itself
template<unsigned Length, unsigned Order, unsigned Dim>
class SavitzkyGolay
{
	BOOST_STATIC_ASSERT(Length > Order);

public:
	typedef boost::array<math::Vector, Dim> vector_array;

public:
	SavitzkyGolay()
	{
		// Init vectors
		std::for_each(std::begin(values_), std::end(values_),
			[] (math::Vector & v) -> void { v.resize(Length); });
		std::for_each(std::begin(coefs_), std::end(coefs_),
			[] (math::Vector & v) -> void { v.resize(Order + 1); });
	}

protected:
	math::Matrix filterMatrix_;
	vector_array values_;
	vector_array coefs_;
};

template<unsigned Length, unsigned Order, unsigned Dim>
class SmoothingSavitzkyGolay : public SavitzkyGolay<Length, Order, Dim>
{
	BOOST_STATIC_ASSERT((Length % 2) == 1);

public:
	SmoothingSavitzkyGolay(math::float_type xStep)
	{
		// Value of center item is 0, so we can optimize calculations later
		int const centerIndex_ = Length / 2;
		math::Vector x(Length);
		for (int i = 0; i < static_cast<int>(Length); ++i) {
			// To get a positive definite matrix, we need to invert the values here
			x(i) = (centerIndex_ - i) * xStep;
		}
		math::make_polynomial_fit_matrix(x, Order, filterMatrix_);
	}

	template<typename VecType>
	void AppendValue(VecType const & value)
	{
		// Append to beginning (inverted, see above for reason)
		for (std::size_t dim = 0; dim < Dim; ++dim) {
			math::shift_vector<1>(values_[dim]);
			values_[dim](0) = value[dim];
		}
	}

	void RunFromValues() { math::fit_polynomials(filterMatrix_, values_, coefs_); }

	template<unsigned N, typename VecType>
	void EvaluateDerivative(VecType & result)
	{
		BOOST_STATIC_ASSERT(N <= Order);

		// We are evaluating at x = 0
		for (std::size_t dim = 0; dim < Dim; ++dim) {
			result[dim] = math::derivative_coef_c<N, N>::value * coefs_[dim](N);
		}
	}
};

template<unsigned Length, unsigned Order>
class SavitzkyGolayPeakDetector : private SmoothingSavitzkyGolay<Length, Order, 1>
{
public:
	enum PeakType
	{
		None,
		Min,
		Max
	};

	SavitzkyGolayPeakDetector(int suppressionTime = 0)
		: SmoothingSavitzkyGolay(1.0) // We don't care about time here...
		, suppressionTime_(suppressionTime)
		, suppressionTimer_(0)
		, prevDirection_(0)
	{}

	PeakType Run(math::float_type value)
	{
		AppendValue(&value);

		if (--suppressionTimer_ > 0) { return None; }

		RunFromValues();

		math::float_type der[1];
		EvaluateDerivative<1>(der);
		int direction = math::sgn(der[0]);

		int prev = prevDirection_;
		prevDirection_ = direction;

		if (prev == -1 && direction == 1) {
			suppressionTimer_ = suppressionTime_;
			return Min;
		} else if (prev == 1 && direction == -1) {
			suppressionTimer_ = suppressionTime_;
			return Max;
		}

		return None;
	}

private:
	int const suppressionTime_;
	int suppressionTimer_;
	int prevDirection_;
};

} // namespace cf
