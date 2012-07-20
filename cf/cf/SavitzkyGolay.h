#pragma once

#include <boost/array.hpp>
#include <boost/static_assert.hpp>

#include "cf/math.h"

namespace cf {

// Base class, not useful by itself
template<unsigned Length, unsigned Order, unsigned Dim>
class SavitzkyGolay
{
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
	BOOST_STATIC_ASSERT(Length > Order);
	BOOST_STATIC_ASSERT((Length % 2) == 1);

public:
	SmoothingSavitzkyGolay(math::float_type xStep)
	{
		// Center item is 0, so we can optimize calculations later
		unsigned const centerIndex_ = Length / 2 +1;
		math::Vector x(Length);
		for (unsigned i = 0; i < Length; ++i) {
			x(i) = (i + 1 - centerIndex_) * xStep;
		}
		math::make_polynomial_fit_matrix(x, Order, filterMatrix_);
	}

	template<typename VecType>
	void AppendValue(VecType const & value)
	{
		for (std::size_t dim = 0; dim < Dim; ++dim) {
			math::shift_vector<-1>(values_[dim]);
			values_[dim](Length - 1) = value[dim];
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


} // namespace cf
