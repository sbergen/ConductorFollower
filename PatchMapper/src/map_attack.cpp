#include "mappers.h"

namespace cf {
namespace PatchMapper {

double map_attack(MappingContext const & context)
{
	// TODO hmm? Why is this not in use?
	// When weight is high, attack shouldn't be quite as high...
	double weightCorrection = -context.conductorContext.weight / 5.0;

	time_quantity legatoThreshold = 0.005 * si::seconds;
	double stacatoPrevention = 0.0;
	if (context.noteContext.timeToNext < legatoThreshold) {
		stacatoPrevention = -0.2;
	}

	return context.conductorContext.attack
		   + stacatoPrevention;
}

} // namespace PatchMapper
} // namespace cf
