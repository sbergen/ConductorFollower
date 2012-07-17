#pragma once

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>

namespace cf {
namespace FeatureExtractor {

namespace ublas = boost::numeric::ublas;

template<class T>
bool inverse_matrix(ublas::matrix<T> const input, ublas::matrix<T>& inverse)
{
	using namespace ublas;

	// create a working copy of the input and a permutation matrix
	matrix<T> A(input);
	permutation_matrix<std::size_t> pm(A.size1());

	// perform LU-factorization
	int result = lu_factorize(A, pm);
	if( result != 0 ) { return false; }

	// create identity matrix of "inverse"
	inverse.assign(ublas::identity_matrix<T>(A.size1()));
	
	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);
	return true;
}

} // namespace FeatureExtractor
} // namespace cf