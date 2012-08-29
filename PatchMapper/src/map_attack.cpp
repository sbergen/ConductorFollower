#include "mappers.h"

namespace cf {
namespace PatchMapper {

double map_attack(MappingContext const & context)
{
	// When weight is high, attack shouldn't quite as high...
	double weightCorrection = -context.conductorContext.weight / 5.0;
	return context.conductorContext.attack;
}

} // namespace PatchMapper
} // namespace cf
