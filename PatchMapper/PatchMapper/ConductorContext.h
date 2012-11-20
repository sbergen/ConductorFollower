#pragma once 

#include "cf/time.h"

namespace cf {
namespace PatchMapper {

struct ConductorContext
{
	ConductorContext()
		: tempo(1.0)
		, weight(0.5)
		, attack(0.5)
		, velocity(0.5)
		, expressionAmount(0.5)
	{}

	double tempo;
	double weight;
	double attack;
	double velocity;

	double expressionAmount;
};

} // namespace PatchMapper
} // namespace cf