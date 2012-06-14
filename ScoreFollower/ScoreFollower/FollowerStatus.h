#pragma once

#include "cf/ChangeTracked.h"
#include "cf/StatusGroup.h"

namespace cf {
namespace ScoreFollower {
namespace Status {

CF_STATUS_GROUP
(
FollowerStatus,
	(Running)("Running")(bool)
	(Speed)("Speed")(double)
)

} // namespace Status
} // namespace ScoreFollower
} // namespace cf
