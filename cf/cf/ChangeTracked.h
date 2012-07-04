#pragma once

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

private:
	mutable bool changed_;
	ValueType value_;
};

} // namespace cf