#pragma once

#include <climits>
#include <map>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

namespace cf {

template<typename TValue, typename TAssignable, TAssignable DefaultValue, TAssignable MinValue, TAssignable MaxValue>
class OptionBase
{
	BOOST_STATIC_ASSERT(MinValue <= MaxValue);
	BOOST_STATIC_ASSERT(MinValue <= DefaultValue);
	BOOST_STATIC_ASSERT(DefaultValue <= MaxValue);

public:
	typedef TAssignable assignable_type;
	typedef TValue value_type;
	typedef boost::is_enum<TAssignable> is_enum;
	typedef boost::is_same<TValue, bool> is_bool;
	typedef boost::is_integral<TValue> is_integral;

	enum {
		default_value = DefaultValue,
		min_value = MinValue,
		max_value = MaxValue
	};

	OptionBase() : value_(DefaultValue) {}

	void setValue(TValue const & value)
	{
		if (value < MinValue) { value_ = MinValue; }
		else if (value > MaxValue) { value_ = MaxValue; }
		else { value_ = value; }
	}

	TValue const & value() const { return value_; }

private:
	TValue value_;
};

template<typename TValue, TValue DefaultValue, TValue MinValue, TValue MaxValue>
class Option : public OptionBase<TValue, TValue, DefaultValue, MinValue, MaxValue> {};

// shorthand for Boost.Enums
template<typename TValue, typename TValue::domain DefaultValue>
class EnumOption : public OptionBase<TValue, typename TValue::domain, DefaultValue,
	static_cast<typename TValue::domain>(0),
	static_cast<typename TValue::domain>(TValue::size - 1)>
{};

// Shorthand for bool
template<bool DefaultValue>
class BooleanOption : public Option<bool, DefaultValue, false, true>
{};

// Shorthand for double
template<int DefaultValue, int MinValue, int MaxValue>
class FloatOption : public OptionBase<double, int, DefaultValue, MinValue, MaxValue>
{};

} // namespace cf
