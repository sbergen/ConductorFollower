#pragma once

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "cf/EventBuffer.h"
#include "cf/functional.h"
#include "cf/math.h"
#include "cf/time.h"

namespace cf {

template<typename T, typename Multiplier = coef_multiply<double, T> >
class TimeFilter
{
public:
	typedef boost::function<double(time_quantity const &)> CoefFunction;

	TimeFilter(size_t length, CoefFunction function)
		: buffer_(length)
		, function_ (function)
	{}

	void Reset(timestamp_t const & time, T const & value)
	{
		buffer_.Clear();
		AddValue(time, value);
	}

	void AddValue(timestamp_t const & time, T const & value)
	{
		buffer_.RegisterEvent(time, value);
	}

	T ValueAt(timestamp_t const & time) const
	{
		T value = T();
		double coefSum = 0.0;
		buffer_.AllEvents().ForEach(
			[&](timestamp_t const & ts, T const & data)
			{
				auto timeSince = time_cast<time_quantity>(time - ts);
				auto coef = function_(timeSince);
				coefSum += coef;
				value += multiplier_(coef, data);
			});

		if (coefSum == 0.0) { return value; }

		auto correction = 1.0 / coefSum;
		return multiplier_(correction, value);
	}

	T ValueNow() const
	{
		return ValueAt(time::now());
	}

private:
	EventBuffer<T, timestamp_t> buffer_;
	CoefFunction function_;
	Multiplier multiplier_;
};

template<typename T, typename Multiplier = coef_multiply<double, T> >
class LinearDecayTimeFilter : public TimeFilter<T, Multiplier>
{
public:
	LinearDecayTimeFilter(size_t length, time_quantity cutoffPoint)
		: TimeFilter(length, boost::bind(&LinearDecayTimeFilter::CoefFunction, this, _1))
		, cutoffPoint_(cutoffPoint)
	{}

private:
	double CoefFunction(time_quantity const & time) const
	{
		if (time > cutoffPoint_) { return 0.0; }
		if (time < time_quantity::from_value(0.0)) { return 0.0; }

		return 1.0 - (time / cutoffPoint_);
	}

	time_quantity cutoffPoint_;
};

} // namespace cf
