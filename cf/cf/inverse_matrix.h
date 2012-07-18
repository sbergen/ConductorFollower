#pragma once

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>

namespace cf {
namespace math {

namespace ublas = boost::numeric::ublas;

template<class M>
bool inverse_matrix(M const & input, M & inverse)
{
	using namespace ublas;

	// create a working copy of the input and a permutation matrix
	typename M::matrix_temporary_type A(input);
	permutation_matrix<std::size_t, unbounded_array<math::uint_type, math::uint_allocator_type> > pm(A.size1());

	// perform LU-factorization
	int result = lu_factorize(A, pm);
	if( result != 0 ) { return false; }

	// create identity matrix of "inverse"
	inverse.assign(ublas::identity_matrix<typename M::value_type, typename M::array_type::allocator_type>(A.size1()));
	
	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);
	return true;
}

} // namespace math
} // namespace cf