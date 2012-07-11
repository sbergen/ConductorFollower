#pragma once

#include <string>

namespace cf {
namespace Data {

struct InstrumentPatch
{
	std::string name;
	int keyswitch;

	double length;
	double attack;
	double weight;
};

} // namespace Data
} // namespace cf
