#pragma once

#include <cmath>
#include <vector>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/storage.hpp>

#include "cf/TlsfAllocator.h"

namespace cf {
namespace math {

namespace detail {
	enum { float_pool_size = 2048u, uint_pool_size = 512u };
}

// sign function
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// Numeric types
typedef double float_type;
typedef std::size_t uint_type;

// Allocator and array types
namespace ublas = boost::numeric::ublas;
typedef TlsfAllocator<float_type> float_allocator_type;
typedef TlsfAllocator<uint_type> uint_allocator_type;
typedef ublas::unbounded_array<float_type, float_allocator_type> math_array;

// Vector and matrix types
typedef ublas::vector<float_type, math_array> Vector;
typedef ublas::matrix<float_type, ublas::row_major, math_array> Matrix;

// center element of vector
inline float_type & center_element(Vector & v)
{
	auto size = v.size();
	assert((size % 2) == 1);
	// truncate and add one
	return v((size / 2) + 1);
}

// Generic random access container wchich uses rt safe allocator
// should be a template alias...
template<typename T>
struct container : public std::vector<T, TlsfAllocator<T> >
{
	container(std::size_t count) : vector(count) {}
};

} // namespace math
} // namespace cf