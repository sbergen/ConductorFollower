#pragma once

#include <boost/enum.hpp>

#include "Limited.h"
#include "ChangeTracked.h"

namespace cf {

BOOST_ENUM(
StatusType,
	(Information)
	(Setting)
)

BOOST_ENUM(
StatusPresentation,
	(Boolean)
	(Text)
	(Bar)
)

template<StatusType::domain Type, StatusPresentation::domain Presentation,
	typename TValue, typename TAssignable, TAssignable DefaultValue, TAssignable MinValue, TAssignable MaxValue>
struct StatusItemBase : public ChangeTracked<Limited<TValue, TAssignable, DefaultValue, MinValue, MaxValue> >
{
	enum
	{
		status_type = Type,
		presentation = Presentation
	};

	using ChangeTracked::operator=;
};

// When TValue == TAssignable
template<StatusType::domain Type, StatusPresentation::domain Presentation,
	typename TValue, TValue DefaultValue, TValue MinValue, TValue MaxValue>
class StatusItem : public StatusItemBase<Type, Presentation, TValue, TValue, DefaultValue, MinValue, MaxValue>
{
	using StatusItemBase::operator=;
};

// Shorthand for bool
template<StatusType::domain Type, bool DefaultValue>
class BooleanStatusItem : public StatusItem<Type, StatusPresentation::Boolean, bool, DefaultValue, false, true>
{
	using StatusItem::operator=;
};

// Shorthand for double
template<StatusType::domain Type, StatusPresentation::domain Presentation,
	int DefaultValue, int MinValue, int MaxValue>
class FloatStatusItem : public StatusItemBase<Type, Presentation, double, int, DefaultValue, MinValue, MaxValue>
{
	using StatusItemBase::operator=;
};

} // namespace cf
