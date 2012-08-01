#pragma once

#include <boost/serialization/access.hpp>

namespace cf {

template<typename ValueType>
class ChangeTracked
{
public:
	typedef ValueType value_type;

public:
	ChangeTracked() : changed_(false), value_() {}
	explicit ChangeTracked(ValueType const & value) : changed_(false), value_(value) {}

	template<typename Y>
	bool LoadIfChanged(Y & value) const
	{
		if (changed_) {
			value = value_;
			changed_ = false;
			return true;
		}
		return false;
	}

	operator ValueType() const { return value_; }

	template<typename Y>
	ChangeTracked & operator= (Y const & value)
	{
		if (value_ != value) {
			value_ = value;
			changed_ = true;
		}
		return *this;
	}

	ChangeTracked & operator= (ChangeTracked const & other)
	{
		value_ = other.value_;
		changed_ = other.changed_;
		return *this;
	}

private:
	mutable bool changed_;
	ValueType value_;

private: // Serialization
	friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int /*version*/)
    {
        ar & value_;
		if (Archive::is_loading::value) {
			changed_ = true;
		}
    }
};

} // namespace cf