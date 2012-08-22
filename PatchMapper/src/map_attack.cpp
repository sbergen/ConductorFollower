#include "mappers.h"

namespace cf {
namespace PatchMapper {

double map_attack(MappingContext const & context)
{
	return context.conductorContext.attack;
}

} // namespace PatchMapper
} // namespace cf
