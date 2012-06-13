#pragma once

#include <climits>
#include <map>

#include <boost/static_assert.hpp>

namespace cf {
namespace Options {

template<typename TValue, TValue DefaultValue, TValue MinValue, TValue MaxValue>
class Option
{
	BOOST_STATIC_ASSERT(MinValue <= MaxValue);
	BOOST_STATIC_ASSERT(MinValue <= DefaultValue);
	BOOST_STATIC_ASSERT(DefaultValue <= MaxValue);

public:
	typedef TValue value_type;

	enum {
		default_value = DefaultValue,
		min_value = MinValue,
		max_value = MaxValue
	};

	Option() : value_(DefaultValue) {}

	void setValue(TValue const & value)
	{
		if (value < MinValue) { value_ = MinValue; }
		else if (value > MaxValue) { value_ = MaxValue; }
		else { value_ = value; }
	}

	//std::string const & description() const { return ; }
	TValue const & value() const { return value_; }

private:
	TValue value_;
};

// shorthand for Boost.Enums
template<typename TValue, typename TValue::domain DefaultValue>
class EnumOption : public Option<typename TValue::domain, DefaultValue,
	static_cast<typename TValue::domain>(0),
	static_cast<typename TValue::domain>(TValue::size - 1)>
{};

// Shorthand for bool
template<bool DefaultValue>
class BooleanOption : public Option<bool, DefaultValue, false, true>
{};

// Helper for converting to and from floats, has to be more "run-timey"
template<typename OptionType, typename FloatType = double>
class FloatProxy
{
	enum {
		min_value_ = OptionType::min_value,
		range_ = OptionType::max_value - OptionType::min_value
	};
	typedef typename OptionType::value_type value_type;

public:
	FloatProxy(OptionType & option, FloatType min, FloatType max)
		: option_(option)
		, floatMin_(min)
		, floatRange_(max - min)
	{
		assert(max >= min);
	}

	void setValue(FloatType value)
	{
		FloatType fraction = (value - floatMin_) / floatRange_;
		value_type diff = static_cast<value_type>(fraction * range_);
		option_.setValue(min_value_ + diff);
	}

	FloatType value()
	{
		FloatType fraction = static_cast<FloatType>(option_.value() - min_value_) / range_;
		return floatMin_ + (fraction * floatRange_);
	}

private:
	OptionType & option_;
	FloatType floatMin_;
	FloatType floatRange_;
};

} // namespace Options
} // namespace cf
