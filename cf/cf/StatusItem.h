#pragma once

#include <string>

#include "cf/Limited.h"
#include "cf/Banger.h"
#include "cf/ValueWrapper.h"

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
		Bang // as in Max/MSP
	};

	enum Level
	{
		Basic,
		Advanced
	};

} // namespace Status

template<Status::Type Type, Status::Presentation Presentation, Status::Level Level>
struct StatusItemTags
{
	enum
	{
		status_type = Type,
		presentation = Presentation,
		status_level = Level
	};
};

template<Status::Type Type, Status::Presentation Presentation, Status::Level Level, typename TValue>
struct StatusItemBase
	: public ValueWrapper<TValue>
	, public StatusItemTags<Type, Presentation, Level>
{
	// Assignment forwarding
	StatusItemBase & operator= (TValue const & value)
	{
		return static_cast<StatusItemBase &>(ValueWrapper::operator=(value));
	}

	StatusItemBase & operator= (StatusItemBase const & other)
	{
		return static_cast<StatusItemBase &>(ValueWrapper::operator=(static_cast<ValueWrapper const &>(other)));
	}
};

template<Status::Type Type, Status::Presentation Presentation, Status::Level Level,
	typename TValue, typename TAssignable, TAssignable DefaultValue, TAssignable MinValue, TAssignable MaxValue>
struct LimitedStatusItemBase
	: public Limited<TValue, TAssignable, DefaultValue, MinValue, MaxValue>
	, public StatusItemTags<Type, Presentation, Level>
{
	// Assignment forwarding
	LimitedStatusItemBase & operator= (TValue const & value)
	{
		return static_cast<LimitedStatusItemBase &>(Limited::operator=(value));
	}

	LimitedStatusItemBase & operator= (LimitedStatusItemBase const & other)
	{
		return static_cast<LimitedStatusItemBase &>(Limited::operator=(static_cast<Limited const &>(other)));
	}
};

// When TValue == TAssignable
template<Status::Type Type, Status::Presentation Presentation, Status::Level Level,
	typename TValue, TValue DefaultValue, TValue MinValue, TValue MaxValue>
struct LimitedStatusItem : public LimitedStatusItemBase<Type, Presentation, Level, TValue, TValue, DefaultValue, MinValue, MaxValue>
{
	using LimitedStatusItemBase::operator=;
};

// Shorthand for bool
template<Status::Type Type, Status::Level Level, bool DefaultValue>
struct BooleanStatusItem : public LimitedStatusItem<Type, Status::Boolean, Level, bool, DefaultValue, false, true>
{
	using LimitedStatusItem::operator=;
};

// Shorthand for bang
template<Status::Type Type, Status::Level Level>
struct BangStatusItem
	: public StatusItemTags<Type, Status::Bang, Level>
	, public Banger
{};

// Shorthand for double
template<Status::Type Type, Status::Presentation Presentation, Status::Level Level,
	int DefaultValue, int MinValue, int MaxValue>
struct FloatStatusItem : public LimitedStatusItemBase<Type, Presentation, Level, double, int, DefaultValue, MinValue, MaxValue>
{
	using LimitedStatusItemBase::operator=;
};

// Shorthand for string
template<Status::Type Type, Status::Presentation Presentation, Status::Level Level>
struct StringStatusItem : public StatusItemBase<Type, Presentation, Level, std::string>
{
	using StatusItemBase::operator=;
};


// Shorthand for StringEnum types
template<Status::Type Type, Status::Presentation Presentation, Status::Level Level, typename TValue, typename TValue::Value DefaultValue>
struct EnumStatusItem
	: public LimitedStatusItemBase<Type, Presentation, Level, TValue, typename TValue::Value, DefaultValue,
                                   static_cast<typename TValue::Value> (0), static_cast<typename TValue::Value> (TValue::size)>
{
	using LimitedStatusItemBase::operator=;
};

} // namespace cf
