#pragma once

#include "cf/StatusItem.h"

#include "Components/OptionWidget.h"
#include "Components/InformationWidget.h"

// Select from InformationWidget or OptionWidget in specializations
template<typename KeyType, typename ValueType, int StatusType>
class StatusWidgetSelector {};

// Expand template parameters to include status_type
template<typename KeyType, typename ValueType>
class StatusWidget
	: public StatusWidgetSelector<KeyType, ValueType, ValueType::status_type> {};

// Common base for tags
template<typename ValueType>
struct StatusWidgetTags
{
	cf::Status::Level level() const { return static_cast<cf::Status::Level>(ValueType::status_level); }
};

// Specializations, which are actually used are below:

// Specialization for InformationWidget
template<typename KeyType, typename ValueType>
class StatusWidgetSelector<KeyType, ValueType, cf::Status::Information>
	: public InformationWidget<KeyType, ValueType, ValueType::presentation>
	, public StatusWidgetTags<ValueType>
{};

// Specialization for OptionWidget
template<typename KeyType, typename ValueType>
class StatusWidgetSelector<KeyType, ValueType, cf::Status::Setting>
	: public OptionWidget<KeyType, ValueType, ValueType::presentation>
	, public StatusWidgetTags<ValueType>
{};
