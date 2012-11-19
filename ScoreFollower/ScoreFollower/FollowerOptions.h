#pragma once

#include "cf/StatusGroup.h"

namespace cf {
namespace ScoreFollower {
namespace Options {

typedef StringStatusItem<cf::Status::Setting, cf::Status::File, cf::Status::Basic> FileType;
typedef BangStatusItem<cf::Status::Setting, cf::Status::Basic> BasicBangType;
typedef FloatStatusItem<cf::Status::Setting, cf::Status::Bar, cf::Status::Advanced, 15, 0, 100> CatchupFractionType;
typedef FloatStatusItem<cf::Status::Setting, cf::Status::Bar, cf::Status::Advanced, 900, 100, 3000> TimeFilterCoefType;
typedef FloatStatusItem<cf::Status::Setting, cf::Status::Bar, cf::Status::Advanced, 50, 0, 100> TempoChangeFollowingType;

CF_STATUS_GROUP
(
FollowerOptions,
	((ScoreDefinition, "Score definition file", FileType))
	((Restart, "Restart score", BasicBangType))
	((Listen, "Listen to score", BasicBangType))
	((CatchupFraction, "Cathup fraction (%)", CatchupFractionType))
	((TempoFilterTime, "Tempo filter cutoff time (ms)", TimeFilterCoefType))
	((TempoChangeFollowing, "Tempo change following (%)", TempoChangeFollowingType))
)

} // namespace Options
} // namespace ScoreFollower
} // namespace cf
