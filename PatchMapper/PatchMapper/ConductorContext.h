#pragma once 

#include "cf/time.h"

namespace cf {
namespace PatchMapper {

struct ConductorContext
{
	ConductorContext()
		: tempo(1.0)
		, power(0.5)
	{}

	double tempo;
	double power;
};

} // namespace PatchMapper
} // namespace cf