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

template<typename TGroup, StatusType::domain Type, typename TValue, typename TAssignable, TAssignable DefaultValue, TAssignable MinValue, TAssignable MaxValue>
struct StatusItemGrouped : public ChangeTracked<TGroup, Limited<TValue, TAssignable, DefaultValue, MinValue, MaxValue> >
{
	enum { status_type = Type };
};

template<StatusType::domain Type, typename TValue, typename TAssignable, TAssignable DefaultValue, TAssignable MinValue, TAssignable MaxValue>
struct StatusItemBase
{
	template<typename TGroup>
	struct grouped { typedef StatusItemGrouped<TGroup, Type, TValue, TAssignable, DefaultValue, MinValue, MaxValue> type; };
};

// When TValue == TAssignable
template<StatusType::domain Type, typename TValue, TValue DefaultValue, TValue MinValue, TValue MaxValue>
class StatusItem : public StatusItemBase<Type, TValue, TValue, DefaultValue, MinValue, MaxValue> {};

// Shorthand for bool
template<bool DefaultValue>
class BooleanStatusItem : public StatusItem<StatusType::Boolean, bool, DefaultValue, false, true> {};

// Shorthand for double
template<StatusType::domain Type, int DefaultValue, int MinValue, int MaxValue>
class FloatStatusItem : public StatusItemBase<Type, double, int, DefaultValue, MinValue, MaxValue> {};

} // namespace cf
