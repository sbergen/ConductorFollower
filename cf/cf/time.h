#pragma once

#include <boost/chrono.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/is_quantity_of_dimension.hpp>

namespace cf {

typedef boost::chrono::steady_clock::time_point timestamp_t;
typedef boost::chrono::steady_clock::duration duration_t;
typedef boost::chrono::milliseconds milliseconds_t;
typedef boost::chrono::microseconds microseconds_t;
typedef boost::chrono::duration<double> seconds_t;

typedef std::pair<timestamp_t, timestamp_t> timespan_t;

typedef boost::units::quantity<boost::units::si::time> time_quantity;

// Casts between Boost.Chrono and Boost.Units representations

// From Chorono to Units
template<typename T, typename Y>
T time_cast(Y y, boost::mpl::true_, boost::mpl::false_)
{
	return T(boost::chrono::duration_cast<seconds_t>(y).count() * boost::units::si::seconds);
}

// From Units to Chrono
template<typename T, typename Y>
T time_cast(Y y, boost::mpl::false_, boost::mpl::true_)
{
	return boost::chrono::duration_cast<T>(seconds_t(y.value()));
}

// Dispatcher
template<typename T, typename Y> T time_cast(Y y)
{
	return time_cast<T>(y,
		boost::units::is_quantity_of_dimension<T, boost::units::time_dimension>(),
		boost::units::is_quantity_of_dimension<Y, boost::units::time_dimension>());
}

namespace time {

inline timestamp_t now() { return boost::chrono::steady_clock::now(); }

template<typename TTime, typename TDuration>
void limitRange(TTime & time, TTime const & min, TTime const & max, TDuration assertThreshold = TDuration::zero())
{
	if (time < min) {
		assert(boost::chrono::duration_cast<TDuration>(min - time) < assertThreshold);
		time = min;
	} else if (time > max) {
		assert(boost::chrono::duration_cast<TDuration>(time - max) < assertThreshold);
		time = max;
	}
}

} // namespace time

} // namespace cf
