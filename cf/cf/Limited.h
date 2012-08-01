#pragma once

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <boost/serialization/access.hpp>

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

	operator TValue() const { return value_; }

	template<typename Y>
	Limited & operator= (Y const & value)
	{
		if (value < MinValue) { value_ = MinValue; }
		else if (value > MaxValue) { value_ = MaxValue; }
		else { value_ = value; }
		return *this;
	}

	Limited & operator= (Limited const & other)
	{
		value_ = other.value_;
		return *this;
	}
	
	bool operator== (TValue const & value) const { return value_ == value; }
	bool operator!= (TValue const & value) const { return value_ != value; }

private:
	TValue value_;

private: // Serialization
	friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int /*version*/)
    {
        ar & value_;
    }
};

} // namespace cf
