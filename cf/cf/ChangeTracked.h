#pragma once

namespace cf {

template<typename GroupParent, typename ValueType>
class ChangeTracked
{
public:
	typedef ValueType value_type;

public:
	ChangeTracked() : changed_(false), value_() {}
	ChangeTracked(ValueType const & value) : changed_(false), value_(value) {}

	template<typename Y>
	void setValue(Y const & value)
	{
		if (value == value_) { return; }
		value_ = value;
		changed_ = true;
		ChangeTrackedGroup<GroupParent>::Changed();
	}

	ValueType const & value() const { return value_; }

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
		setValue(value);
		return *this;
	}

private:
	mutable bool changed_;
	ValueType value_;
};

template<typename ParentType>
class ChangeTrackedGroup
{
public:
	static bool HasSomethingChanged(bool reset = true)
	{
		bool ret = changed;
		if (reset) { changed = false; }
		return ret;
	}

	static void Changed() { changed = true; }

private:
	static bool changed;
};

template<typename T>
bool ChangeTrackedGroup<T>::changed = false;

} // namespace cf