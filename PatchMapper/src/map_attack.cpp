#include "mappers.h"

namespace cf {
namespace PatchMapper {

double map_attack(MappingContext const & context)
{
	return context.conductorContext.power;
}

} // namespace PatchMapper
} // namespace cf
