#pragma once

#include "cf/StatusItem.h"
#include "cf/StatusGroup.h"
#include "cf/StringEnum.h"

namespace cf {
namespace ScoreFollower {

// TODO ambiguous name, come up with a better one
CF_STRING_ENUM(FollowerState,
	((StartingUp, "Starting up..."))
	((WaitingForCalibration, "Waiting for wave gesture"))
	((WaitingForStart, "Wait for start gesture"))
	((Rolling, "Rolling"))
	((Playback, "Playback"))
	((Stopped, "Stopped"))
)

namespace Status {

typedef FloatStatusItem<cf::Status::Information, cf::Status::Bar, cf::Status::Advanced, 1, 0, 2> SpeedType;
typedef EnumStatusItem<cf::Status::Information, cf::Status::Text, cf::Status::Basic, FollowerState, FollowerState::Stopped> StateType;

typedef FloatStatusItem<cf::Status::Information, cf::Status::Bar, cf::Status::Advanced, 0, 0, 150> MotionLengthType;
typedef FloatStatusItem<cf::Status::Information, cf::Status::Bar, cf::Status::Advanced, 0, 0, 300> VelocityRangeType;
typedef FloatStatusItem<cf::Status::Information, cf::Status::Bar, cf::Status::Advanced, 0, 0, 150> JerkPeakType;

CF_STATUS_GROUP
(
FollowerStatus,
	((State, "State", StateType))
	((Speed, "Speed", SpeedType))
	((MotionLength, "Motion length", MotionLengthType))
	((VelocityRange, "Velocity dynamic range", VelocityRangeType))
	((JerkPeak, "Jerk peak", JerkPeakType))
)

} // namespace Status
} // namespace ScoreFollower
} // namespace cf
