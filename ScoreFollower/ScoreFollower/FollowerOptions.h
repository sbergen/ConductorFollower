#pragma once

#include "cf/StatusGroup.h"

namespace cf {
namespace ScoreFollower {
namespace Options {

typedef FloatStatusItem<StatusType::Setting, StatusPresentation::Bar, 10, 0, 20> TempoFromPhaseThreshType;

CF_STATUS_GROUP
(
FollowerOptions,
	((TempoFromPhaseThresh, "Tempo diff threshold for phase following (%)", TempoFromPhaseThreshType))
)

} // namespace Options
} // namespace ScoreFollower
} // namespace cf
