#pragma once

#include "cf/StatusGroup.h"

namespace cf {
namespace ScoreFollower {
namespace Options {

typedef StringStatusItem<cf::Status::Setting, cf::Status::File> FileType;
typedef BangStatusItem<cf::Status::Setting> BangType;

CF_STATUS_GROUP
(
FollowerOptions,
	((ScoreDefinition, "Score definition file", FileType))
	((Restart, "Restart score", BangType))
)

} // namespace Options
} // namespace ScoreFollower
} // namespace cf
