#pragma once

#include <string>
#include <vector>

#include "Data/InstrumentPatch.h"

namespace cf {
namespace Data {

struct Instrument
{
	std::string name;
	double shortest_note_threshold;
	double longest_note_threshold;
	std::vector<InstrumentPatch> patches;
};

} // namespace Data
} // namespace cf
