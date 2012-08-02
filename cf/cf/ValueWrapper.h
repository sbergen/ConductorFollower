#pragma once

#include <boost/serialization/access.hpp>

namespace cf {

template<typename ValueType>
class ValueWrapper
{
public:
	typedef ValueType value_type;

public:
	ValueWrapper() : value_() {}
	explicit ValueWrapper(ValueType const & value) : value_(value) {}

	operator ValueType() const { return value_; }

	template<typename Y>
	ValueWrapper & operator= (Y const & value)
	{
		value_ = value;
		return *this;
	}

	ValueWrapper & operator= (ValueWrapper const & other)
	{
		value_ = other.value_;
		return *this;
	}

private:
	ValueType value_;

private: // Serialization
	friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int /*version*/)
    {
        ar & value_;
    }
};

} // namespace cf