#pragma once

#include "cf/StatusItem.h"
#include "cf/StatusGroup.h"

namespace cf {
namespace ScoreFollower {
namespace Status {

typedef FloatStatusItem<StatusType::Bar, 1, 0, 2> SpeedType;

CF_STATUS_GROUP
(
FollowerStatus,
	(Running)("Running")(BooleanStatusItem<false>)
	(Speed)("Speed")(SpeedType)
	(SpeedFromTempo)("Speed from tempo")(SpeedType)
	(SpeedFromPhase)("Speed from phase")(SpeedType)
)

} // namespace Status
} // namespace ScoreFollower
} // namespace cf
