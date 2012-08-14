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
	((GotStart, "Got start gesture"))
	((Rolling, "Rolling"))
	((Stopped, "Stopped"))
)

namespace Status {

typedef BooleanStatusItem<cf::Status::Information, false> BooleanTypeFalse;
typedef FloatStatusItem<cf::Status::Information, cf::Status::Bar, 1, 0, 2> SpeedType;
typedef EnumStatusItem<cf::Status::Information, cf::Status::Text, FollowerState, FollowerState::Stopped> StateType;

typedef FloatStatusItem<cf::Status::Information, cf::Status::Bar, 0, -10, 10> BeatType;

typedef FloatStatusItem<cf::Status::Information, cf::Status::Bar, 0, 0, 3> VelocityPeakType;
typedef FloatStatusItem<cf::Status::Information, cf::Status::Bar, 0, 0, 3> VelocityRangeType;
typedef FloatStatusItem<cf::Status::Information, cf::Status::Bar, 0, 0, 200> JerkPeakType;

CF_STATUS_GROUP
(
FollowerStatus,
	((State, "State", StateType))
	((Speed, "Speed", SpeedType))
	((Beat, "Beat Probability", BeatType))
	((VelocityPeak, "Velocity peak", VelocityPeakType))
	((VelocityRange, "Velocity dynamic range", VelocityRangeType))
	((JerkPeak, "Jerk peak", JerkPeakType))
)

} // namespace Status
} // namespace ScoreFollower
} // namespace cf
