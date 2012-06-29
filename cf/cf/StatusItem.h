#pragma once

#include <boost/enum.hpp>

#include "Limited.h"
#include "ChangeTracked.h"

namespace cf {

BOOST_ENUM(
StatusType,
	(Boolean)
	(Text)
	(Bar)
)

template<StatusType::domain Type, typename TValue, typename TAssignable, TAssignable DefaultValue, TAssignable MinValue, TAssignable MaxValue>
struct StatusItemBase : public ChangeTracked<Limited<TValue, TAssignable, DefaultValue, MinValue, MaxValue> >
{
	enum { status_type = Type };
	using ChangeTracked::operator=;
};

// When TValue == TAssignable
template<StatusType::domain Type, typename TValue, TValue DefaultValue, TValue MinValue, TValue MaxValue>
class StatusItem : public StatusItemBase<Type, TValue, TValue, DefaultValue, MinValue, MaxValue>
{
	using StatusItemBase::operator=;
};

// Shorthand for bool
template<bool DefaultValue>
class BooleanStatusItem : public StatusItem<StatusType::Boolean, bool, DefaultValue, false, true>
{
	using StatusItem::operator=;
};

// Shorthand for double
template<StatusType::domain Type, int DefaultValue, int MinValue, int MaxValue>
class FloatStatusItem : public StatusItemBase<Type, double, int, DefaultValue, MinValue, MaxValue>
{
	using StatusItemBase::operator=;
};

} // namespace cf
