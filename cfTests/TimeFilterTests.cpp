#include <boost/test/unit_test.hpp>

#include "cf/TimeFilters.h"

BOOST_AUTO_TEST_SUITE(TimeFilterTests)

using namespace cf;
namespace si = boost::units::si;

timestamp_t make_time(timestamp_t ref, time_quantity offset)
{
	return ref + time_cast<timestamp_t::duration>(offset);
}


BOOST_AUTO_TEST_CASE(ScalingTest)
{
	// Scaling should be independent of coef function,
	// so we can use the linear decay filter here
	LinearDecayTimeFilter<double> f(5, 1.0 * si::seconds);

	auto refTime = time::now();
	auto checkTime = make_time(refTime, 1.0 * si::seconds);
	double const value = 2.5;
	
	f.AddValue(make_time(refTime, 0.1 * si::seconds), value);
	BOOST_CHECK_CLOSE(f.ValueAt(checkTime), value, 0.1);

	f.AddValue(make_time(refTime, 0.2 * si::seconds), value);
	BOOST_CHECK_CLOSE(f.ValueAt(checkTime), value, 0.1);

	f.AddValue(make_time(refTime, 0.8 * si::seconds), value);
	BOOST_CHECK_CLOSE(f.ValueAt(checkTime), value, 0.1);

	f.AddValue(make_time(refTime, 0.95 * si::seconds), value);
	BOOST_CHECK_CLOSE(f.ValueAt(checkTime), value, 0.1);
}

BOOST_AUTO_TEST_CASE(LinearDecayLimitTest)
{
	LinearDecayTimeFilter<double> f(5, 1.0 * si::seconds);

	auto refTime = time::now();
	auto checkTime = make_time(refTime, 1.5 * si::seconds);
	double const value = 2.5;
	
	f.AddValue(make_time(refTime, 0.1 * si::seconds), 2 * value);
	BOOST_CHECK_CLOSE(f.ValueAt(checkTime), 0.0, 0.1);

	f.AddValue(make_time(refTime, 0.2 * si::seconds), 5 * value);
	BOOST_CHECK_CLOSE(f.ValueAt(checkTime), 0.0, 0.1);

	f.AddValue(make_time(refTime, 0.8 * si::seconds), value);
	BOOST_CHECK_CLOSE(f.ValueAt(checkTime), value, 0.1);

	f.AddValue(make_time(refTime, 1.2 * si::seconds), value);
	BOOST_CHECK_CLOSE(f.ValueAt(checkTime), value, 0.1);
}

BOOST_AUTO_TEST_SUITE_END()