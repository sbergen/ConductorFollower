#pragma once

#include <cmath>

#include <boost/numeric/ublas/matrix.hpp>

namespace cf {
namespace FeatureExtractor {
namespace math {

namespace ublas = boost::numeric::ublas;

typedef double float_type;
typedef ublas::vector<float_type> Vector;
typedef ublas::matrix<float_type> Matrix;

} // namespace math
} // namespace FeatureExtractor
} // namespace cf
