#include "mappers.h"

#include "cf/math.h"

namespace cf {
namespace PatchMapper {

double map_attack(MappingContext const & context)
{
	// Expand the dynamic range by not taking into account the lowest values
	double contextAttack = context.conductorContext.attack;
	double const lowerLimit = 0.3;
	contextAttack = (contextAttack - lowerLimit) / (1.0 - lowerLimit);

	// Take velocity into account
	double const velocityFactor = 0.6;
	contextAttack *= 1.0 / ((1.0 - velocityFactor) + velocityFactor * context.conductorContext.velocity);

	time_quantity legatoThreshold = 0.005 * si::seconds;
	double stacatoPrevention = 0.0;
	if (context.noteContext.timeToNext < legatoThreshold) {
		stacatoPrevention = -0.2;
	}

	return scale_to_expression_amount(contextAttack + stacatoPrevention, context);
}

} // namespace PatchMapper
} // namespace cf
