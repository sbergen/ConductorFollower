#pragma once

#include <string>
#include <vector>

#include "Data/InstrumentPatch.h"

namespace cf {
namespace Data {

struct Instrument
{
	std::string name;
	std::vector<InstrumentPatch> patches;
};

} // namespace Data
} // namespace cf
