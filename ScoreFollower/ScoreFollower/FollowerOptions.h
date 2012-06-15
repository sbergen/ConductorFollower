#pragma once

#include "cf/Options.h"
#include "cf/OptionGroup.h"

namespace cf {
namespace ScoreFollower {
namespace Options {

typedef Option<int, 10, 0, 20> TempoFromPhaseThreshType;

CF_OPTION_GROUP
(
FollowerOptions,
	(TempoFromPhaseThresh)("Tempo diff threshold for phase following (%)")(TempoFromPhaseThreshType)
)

} // namespace Options
} // namespace ScoreFollower
} // namespace cf
