#pragma once

#include <boost/chrono.hpp>

namespace cf {

typedef boost::chrono::steady_clock::time_point timestamp_t;
typedef boost::chrono::steady_clock::duration duration_t;
typedef boost::chrono::milliseconds milliseconds_t;
typedef boost::chrono::duration<double> seconds_t;

namespace time {

inline timestamp_t now() { return boost::chrono::steady_clock::now(); }

} // namespace time

} // namespace cf
