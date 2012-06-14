#pragma once

namespace cf {

template<typename GroupParent, typename ValueType>
class ChangeTracked
{
public:
	ChangeTracked() : changed_(false), value_() {}

	template<typename Y>
	void Set(Y const & value)
	{
		if (value == value_) { return; }
		value_ = value;
		changed_ = true;
		ChangeTrackedGroup<GroupParent>::Changed();
	}

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