#pragma once

#include <exception>

#include <boost/variant.hpp>
#include <boost/array.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "cf/time.h"
#include "cf/score_units.h"
#include "cf/math.h"

namespace cf {

namespace bu = boost::units;
namespace ublas = boost::numeric::ublas;

class LogItem
{
private:
	typedef boost::variant<
		boost::blank,
		float, double, 
		int, unsigned, long,
		char const *,
		timestamp_t, duration_t, milliseconds_t, seconds_t, boost::chrono::microseconds,
		bu::quantity<score::musical_time>, bu::quantity<score::physical_time>,
		bu::quantity<score::tempo>, bu::quantity<score::bar_count>, bu::quantity<score::bar_duration>,
		math::Matrix > Arg;

public:
	LogItem()
		: argIndex_(0)
	{
		args_[0] = Arg();
	}

	LogItem(char const * fmt)
		: argIndex_(0)
	{
		for (int i = 0; i < MaxFmtLen; ++i) {
			format_[i] = fmt[i];
			if (fmt[i] == '\0') {
				args_[0] = Arg();
				return;
			}
		}

		throw std::invalid_argument("Too long format string given to Logger");
	}
	
	// The Boost.Format convention
	template<typename T>
	LogItem & operator% (T const & arg)
	{
		args_[argIndex_] = arg;
		++argIndex_;
		assert(argIndex_ < MaxArgs);
		args_[argIndex_] = Arg();
		return *this;
	}

	// For variadric macro convenience
	template<typename T>
	LogItem & operator, (T const & arg)
	{
		return *this % arg;
	}

private:
	friend class LogBuffer;

	enum { MaxFmtLen = 512, MaxArgs = 6 };

	int argIndex_;
	boost::array<std::string::value_type, MaxFmtLen> format_;
	boost::array<Arg, MaxArgs> args_;
};

} // namespace cf
