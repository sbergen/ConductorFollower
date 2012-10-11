#pragma once

#include "cf/StatusGroup.h"

namespace cf {
namespace ScoreFollower {
namespace Options {

typedef StringStatusItem<cf::Status::Setting, cf::Status::File> FileType;
typedef BangStatusItem<cf::Status::Setting> BangType;
typedef FloatStatusItem<cf::Status::Setting, cf::Status::Bar, 35, 0, 100> CatchupFractionType;
typedef FloatStatusItem<cf::Status::Setting, cf::Status::Bar, 2500, 100, 5000> TimeFilterCoefType;

CF_STATUS_GROUP
(
FollowerOptions,
	((ScoreDefinition, "Score definition file", FileType))
	((Restart, "Restart score", BangType))
	((Listen, "Listen to score", BangType))
	((CatchupFraction, "Cathup fraction (%)", CatchupFractionType))
	((TempoFilterTime, "Tempo filter cutoff time (ms)", TimeFilterCoefType))
)

} // namespace Options
} // namespace ScoreFollower
} // namespace cf
