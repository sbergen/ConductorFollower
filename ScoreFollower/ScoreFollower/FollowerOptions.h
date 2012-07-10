#pragma once

#include "cf/StatusGroup.h"

namespace cf {
namespace ScoreFollower {
namespace Options {

typedef StringStatusItem<cf::Status::Setting, cf::Status::File> FileType;

CF_STATUS_GROUP
(
FollowerOptions,
	((MidiFile, "Midi score file", FileType))
	((ScoreDefinition, "Score definition file", FileType))
	((InstrumentDefinitions, "Instrument definitions file", FileType))
)

} // namespace Options
} // namespace ScoreFollower
} // namespace cf
