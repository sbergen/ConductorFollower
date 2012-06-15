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
	(SpeedFromTempo)("Speed from tempo")(double)
	(SpeedFromPhase)("Speed from phase")(double)
)

} // namespace Status
} // namespace ScoreFollower
} // namespace cf
