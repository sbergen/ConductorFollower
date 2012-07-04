#pragma once

#include <string>

#include <boost/enum.hpp>

#include "Limited.h"
#include "ChangeTracked.h"

namespace cf {

namespace Status
{
	enum Type
	{
		Information,
		Setting
	};

	enum Presentation
	{
		Boolean,
		Text,
		Bar,
		File
	};

} // namespace Status

template<Status::Type Type, Status::Presentation Presentation>
struct StatusItemTags
{
	enum
	{
		status_type = Type,
		presentation = Presentation
	};
};

template<Status::Type Type, Status::Presentation Presentation, typename TValue>
struct StatusItemBase
	: public ChangeTracked<TValue>
	, public StatusItemTags<Type, Presentation>
{
	using ChangeTracked::operator=;
};

template<Status::Type Type, Status::Presentation Presentation,
	typename TValue, typename TAssignable, TAssignable DefaultValue, TAssignable MinValue, TAssignable MaxValue>
struct LimitedStatusItemBase
	: public ChangeTracked<Limited<TValue, TAssignable, DefaultValue, MinValue, MaxValue> >
	, public StatusItemTags<Type, Presentation>
{
	using ChangeTracked::operator=;

	// Type conversion forwarding
	typedef Limited<TValue, TAssignable, DefaultValue, MinValue, MaxValue> limited_type;
	operator TValue() const { return static_cast<limited_type const &>(*this); }
};

// When TValue == TAssignable
template<Status::Type Type, Status::Presentation Presentation,
	typename TValue, TValue DefaultValue, TValue MinValue, TValue MaxValue>
class LimitedStatusItem : public LimitedStatusItemBase<Type, Presentation, TValue, TValue, DefaultValue, MinValue, MaxValue>
{
	using LimitedStatusItemBase::operator=;
};

// Shorthand for bool
template<Status::Type Type, bool DefaultValue>
class BooleanStatusItem : public LimitedStatusItem<Type, Status::Boolean, bool, DefaultValue, false, true>
{
	using LimitedStatusItem::operator=;
};

// Shorthand for double
template<Status::Type Type, Status::Presentation Presentation,
	int DefaultValue, int MinValue, int MaxValue>
class FloatStatusItem : public LimitedStatusItemBase<Type, Presentation, double, int, DefaultValue, MinValue, MaxValue>
{
	using LimitedStatusItemBase::operator=;
};

// Shorthand for string
template<Status::Type Type, Status::Presentation Presentation>
class StringStatusItem : public StatusItemBase<Type, Presentation, std::string>
{
	using StatusItemBase::operator=;
};


} // namespace cf
