#pragma once

namespace cf {

template<typename T>
class ChangeTracked
{
public:
	ChangeTracked() : changed_(false), globalChanged_(nullptr) {}

	template<typename Y>
	ChangeTracked(Y value) : changed_(false), globalChanged_(nullptr), value_(value) {}

	template<typename Y>
	ChangeTracked(Y value, bool & globalChanged) : changed_(false), globalChanged_(&globalChanged), value_(value) {}

	template<typename Y>
	ChangeTracked & operator=(Y const & value)
	{
		if (value != value_) {
			value_ = value;
			changed_ = true;
			if (globalChanged_) { *globalChanged_ = true; }
		}
		return *this;
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
	bool * const globalChanged_;
	T value_;
};

} // namespace cf