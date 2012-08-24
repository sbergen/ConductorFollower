#pragma once

#include <string>
#include <vector>

#include "Data/Track.h"
#include "Data/ScoreEvent.h"

namespace cf {
namespace Data {

typedef std::vector<Track> TrackList;
typedef std::vector<ScoreEvent> ScoreEventList;

struct Score
{
	std::string name;
	std::string midiFile;
	std::string instrumentFile;
	std::string beatPatternFile;

	TrackList tracks;
	ScoreEventList events;
};

} // namespace Data
} // namespace cf
