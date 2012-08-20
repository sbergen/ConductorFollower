#pragma once

#include <vector>

#include "cf/TimeSignature.h"

#include "Data/Beat.h"

namespace cf {
namespace Data {

struct BeatPattern
{
	TimeSignature meter;
	std::vector<Beat> beats;
};

} // namespace Data
} // namespace cf