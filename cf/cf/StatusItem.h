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
		File,
		Bang // as in Max
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
	// Assignment forwarding
	StatusItemBase & operator= (TValue const & value)
	{
		return static_cast<StatusItemBase &>(ChangeTracked::operator=(value));
	}

	StatusItemBase & operator= (StatusItemBase const & other)
	{
		return static_cast<StatusItemBase &>(ChangeTracked::operator=(static_cast<ChangeTracked const &>(other)));
	}
};

template<Status::Type Type, Status::Presentation Presentation,
	typename TValue, typename TAssignable, TAssignable DefaultValue, TAssignable MinValue, TAssignable MaxValue>
struct LimitedStatusItemBase
	: public ChangeTracked<Limited<TValue, TAssignable, DefaultValue, MinValue, MaxValue> >
	, public StatusItemTags<Type, Presentation>
{
	// Assignment forwarding
	LimitedStatusItemBase & operator= (TValue const & value)
	{
		return static_cast<LimitedStatusItemBase &>(ChangeTracked::operator=(value));
	}

	LimitedStatusItemBase & operator= (LimitedStatusItemBase const & other)
	{
		return static_cast<LimitedStatusItemBase &>(ChangeTracked::operator=(static_cast<ChangeTracked const &>(other)));
	}

	// Type conversion forwarding
	typedef Limited<TValue, TAssignable, DefaultValue, MinValue, MaxValue> limited_type;
	operator TValue() const { return static_cast<limited_type const &>(*this); }
};

// When TValue == TAssignable
template<Status::Type Type, Status::Presentation Presentation,
	typename TValue, TValue DefaultValue, TValue MinValue, TValue MaxValue>
struct LimitedStatusItem : public LimitedStatusItemBase<Type, Presentation, TValue, TValue, DefaultValue, MinValue, MaxValue>
{
	using LimitedStatusItemBase::operator=;
};

// Shorthand for bool
template<Status::Type Type, bool DefaultValue>
struct BooleanStatusItem : public LimitedStatusItem<Type, Status::Boolean, bool, DefaultValue, false, true>
{
	using LimitedStatusItem::operator=;
};

// Shorthand for bang
template<Status::Type Type>
struct BangStatusItem : public LimitedStatusItem<Type, Status::Bang, bool, false, false, true>
{
	using LimitedStatusItem::operator=;
};

// Shorthand for double
template<Status::Type Type, Status::Presentation Presentation,
	int DefaultValue, int MinValue, int MaxValue>
struct FloatStatusItem : public LimitedStatusItemBase<Type, Presentation, double, int, DefaultValue, MinValue, MaxValue>
{
	using LimitedStatusItemBase::operator=;
};

// Shorthand for string
template<Status::Type Type, Status::Presentation Presentation>
struct StringStatusItem : public StatusItemBase<Type, Presentation, std::string>
{
	using StatusItemBase::operator=;
};


// Shorthand for StringEnum types
template<Status::Type Type, Status::Presentation Presentation, typename TValue, typename TValue::Value DefaultValue>
struct EnumStatusItem : public LimitedStatusItemBase<Type, Presentation, TValue, typename TValue::Value, DefaultValue,
                                              static_cast<typename TValue::Value> (0), static_cast<typename TValue::Value> (TValue::size)>
{
	using LimitedStatusItemBase::operator=;
};

} // namespace cf
