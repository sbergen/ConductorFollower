#pragma once

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

namespace cf {

template<typename TValue, typename TAssignable, TAssignable DefaultValue, TAssignable MinValue, TAssignable MaxValue>
class Limited
{
	BOOST_STATIC_ASSERT(MinValue <= MaxValue);
	BOOST_STATIC_ASSERT(MinValue <= DefaultValue);
	BOOST_STATIC_ASSERT(DefaultValue <= MaxValue);

public:
	typedef TAssignable assignable_type;
	typedef TValue value_type;

	// For convenience
	typedef boost::is_enum<TAssignable> is_enum;
	typedef boost::is_same<TValue, bool> is_bool;
	typedef boost::is_integral<TValue> is_integral;

public:
	enum {
		default_value = DefaultValue,
		min_value = MinValue,
		max_value = MaxValue
	};

	Limited() : value_(DefaultValue) {}

	template<typename Y>
	void setValue(Y const & value)
	{
		if (value < MinValue) { value_ = MinValue; }
		else if (value > MaxValue) { value_ = MaxValue; }
		else { value_ = value; }
	}

	TValue const & value() const { return value_; }

	operator TValue() const { return value_; }

	template<typename Y>
	Limited & operator= (Y const & value)
	{
		setValue(value);
		return *this;
	}

private:
	TValue value_;
};

} // namespace cf
