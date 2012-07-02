#pragma once

#include <string>

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
	(File)
)

template<StatusType::domain Type, StatusPresentation::domain Presentation>
struct StatusItemTags
{
	enum
	{
		status_type = Type,
		presentation = Presentation
	};
};

template<StatusType::domain Type, StatusPresentation::domain Presentation, typename TValue>
struct StatusItemBase
	: public ChangeTracked<TValue>
	, public StatusItemTags<Type, Presentation>
{
	using ChangeTracked::operator=;
};

template<StatusType::domain Type, StatusPresentation::domain Presentation,
	typename TValue, typename TAssignable, TAssignable DefaultValue, TAssignable MinValue, TAssignable MaxValue>
struct LimitedStatusItemBase
	: public ChangeTracked<Limited<TValue, TAssignable, DefaultValue, MinValue, MaxValue> >
	, public StatusItemTags<Type, Presentation>
{
	using ChangeTracked::operator=;
};

// When TValue == TAssignable
template<StatusType::domain Type, StatusPresentation::domain Presentation,
	typename TValue, TValue DefaultValue, TValue MinValue, TValue MaxValue>
class LimitedStatusItem : public LimitedStatusItemBase<Type, Presentation, TValue, TValue, DefaultValue, MinValue, MaxValue>
{
	using LimitedStatusItemBase::operator=;
};

// Shorthand for bool
template<StatusType::domain Type, bool DefaultValue>
class BooleanStatusItem : public LimitedStatusItem<Type, StatusPresentation::Boolean, bool, DefaultValue, false, true>
{
	using LimitedStatusItem::operator=;
};

// Shorthand for double
template<StatusType::domain Type, StatusPresentation::domain Presentation,
	int DefaultValue, int MinValue, int MaxValue>
class FloatStatusItem : public LimitedStatusItemBase<Type, Presentation, double, int, DefaultValue, MinValue, MaxValue>
{
	using LimitedStatusItemBase::operator=;
};

// Shorthand for string
template<StatusType::domain Type, StatusPresentation::domain Presentation>
class StringStatusItem : public StatusItemBase<Type, Presentation, std::string>
{
	using StatusItemBase::operator=;
};


} // namespace cf
