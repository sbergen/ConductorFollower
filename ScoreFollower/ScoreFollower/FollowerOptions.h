#pragma once

#include "cf/StatusGroup.h"

namespace cf {
namespace ScoreFollower {
namespace Options {

typedef StringStatusItem<StatusType::Setting, StatusPresentation::File> FileType;

CF_STATUS_GROUP
(
FollowerOptions,
	((InstrumentDefinitions, "Instrument definitions file", FileType))
)

} // namespace Options
} // namespace ScoreFollower
} // namespace cf
