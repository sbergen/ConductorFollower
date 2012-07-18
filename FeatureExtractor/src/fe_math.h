#pragma once

#include <cmath>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/storage.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>

#include "cf/dummy_mutex.h"

namespace cf {
namespace FeatureExtractor {
namespace math {

namespace ublas = boost::numeric::ublas;

// Numeric types
typedef double float_type;
typedef std::size_t uint_type;

// Allocator and array types
typedef boost::pool_allocator<float_type, boost::default_user_allocator_new_delete, dummy_mutex, 2048u, 2048u> float_allocator_type;
typedef boost::pool_allocator<uint_type, boost::default_user_allocator_new_delete, dummy_mutex, 512u, 512u> uint_allocator_type;
typedef ublas::unbounded_array<float_type, float_allocator_type> math_array;

// Vector and matrix types
typedef ublas::vector<float_type, math_array> Vector;
typedef ublas::matrix<float_type, ublas::row_major, math_array> Matrix;

} // namespace math
} // namespace FeatureExtractor
} // namespace cf
