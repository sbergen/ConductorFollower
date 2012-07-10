#pragma once

#include "cf/StatusItem.h"
#include "cf/StatusGroup.h"
#include "cf/StringEnum.h"

namespace cf {
namespace ScoreFollower {

// TODO ambiguous name, come up with a better one
CF_STRING_ENUM(FollowerState,
	((WaitingForCalibration, "Waiting for wave gesture"))
	((WaitingForStart, "Wait for start gesture"))
	((Rolling, "Rolling"))
	((Stopped, "Stopped"))
)

namespace Status {

typedef BooleanStatusItem<cf::Status::Information, false> BooleanTypeFalse;
typedef FloatStatusItem<cf::Status::Information, cf::Status::Bar, 1, 0, 2> SpeedType;
typedef FloatStatusItem<cf::Status::Information, cf::Status::Bar, 150, 50, 600> MagnitudeType;
typedef EnumStatusItem<cf::Status::Information, cf::Status::Text, FollowerState, FollowerState::Stopped> StateType;

CF_STATUS_GROUP
(
FollowerStatus,
	((State, "State", StateType))
	((Speed, "Speed", SpeedType))
	((SpeedFromTempo, "Speed from tempo", SpeedType))
	((SpeedFromPhase, "Speed from phase", SpeedType))
	((MagnitudeOfMovement, "Magnitude of movement (mm)", MagnitudeType))
)

} // namespace Status
} // namespace ScoreFollower
} // namespace cf
