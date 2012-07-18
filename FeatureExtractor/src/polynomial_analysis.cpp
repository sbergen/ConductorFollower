#include "polynomial_analysis.h"

#include "cf/polynomial.h"

namespace cf {
namespace FeatureExtractor {

using namespace cf::math;

namespace {

void collect_data_for_analysis(PositionBuffer::Range const & events,
	Vector & times, boost::array<Vector, 3> & values)
{
	unsigned const size = events.Size();
	
	// Resize
	times.resize(size);
	std::for_each(std::begin(values), std::end(values),
		[size] (Vector & v) -> void { v.resize(size); });

	// Collect
	std::size_t i = 0;
	auto referenceTime = events[0].timestamp;
	events.ForEach([&i, referenceTime, &times, &values] (timestamp_t const & time, Point3D const & point)
	{
		times(i) = time::quantity_cast<time_quantity>(time - referenceTime).value();
		for (std::size_t dim = 0; dim < 3; ++dim) {
			values[dim](i) = point.data()[dim];
		}
		++i;
	});
}

} // anon namespace

PolynomialResult::PolynomialResult(boost::array<math::Vector, 3> const & coefs, math::float_type x)
	: success(true)
{
	for (std::size_t dim = 0; dim < 3; ++dim) {
		Vector const & c = coefs[dim];
		auto d1 = derivative<1>(c);
		auto d2 = derivative<1>(d1);

		// This goes past dimensional analysis, but it's kind of hard to do it anyway...
		interpolatedPoint.data()[dim] = evaluate_polynomial(c, x);
		instantaneousVelocity.data()[dim] = evaluate_polynomial(d1, x);
		instantaneousAcceleration.data()[dim] = evaluate_polynomial(d2, x);
	}
}

PolynomialResult polynomial_analysis(PositionBuffer & buffer)
{
	// extract events
	auto analysisInterval = milliseconds_t(100);
	auto since = buffer.AllEvents().LastTimestamp() - analysisInterval;
	auto events = buffer.EventsSinceInclusive(since);

	// check/adjust size
	if (events.Size() < 5) { return PolynomialResult(); }
	if (events.Size() % 2 == 0) { events.PopFront(); }
	
	// Get vectors for analysis
	Vector times;
	boost::array<Vector, 3> values;
	collect_data_for_analysis(events, times, values);

	// Try to do fitting
	boost::array<Vector, 3> coefs;
	if (!fit_polynomials(2, times, values, coefs)) { return PolynomialResult(); }

	// Construct result
	float_type time = center_element(times);
	return PolynomialResult(coefs, time);
}

} // namespace FeatureExtractor
} // namespace cf
