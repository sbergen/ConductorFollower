#pragma once

#include "cf/Limited.h"

namespace cf {

template<typename TValue, TValue DefaultValue, TValue MinValue, TValue MaxValue>
class Option : public Limited<TValue, TValue, DefaultValue, MinValue, MaxValue>
{
	using Limited::operator=;
};

// shorthand for Boost.Enums
template<typename TValue, typename TValue::domain DefaultValue>
class EnumOption : public Limited<TValue, typename TValue::domain, DefaultValue,
	static_cast<typename TValue::domain>(0),
	static_cast<typename TValue::domain>(TValue::size - 1)>
{
	using Limited::operator=;
};

// Shorthand for bool
template<bool DefaultValue>
class BooleanOption : public Option<bool, DefaultValue, false, true>
{
	using Option::operator=;
};

// Shorthand for double
template<int DefaultValue, int MinValue, int MaxValue>
class FloatOption : public Limited<double, int, DefaultValue, MinValue, MaxValue>
{
	using Limited::operator=;
};

} // namespace cf
