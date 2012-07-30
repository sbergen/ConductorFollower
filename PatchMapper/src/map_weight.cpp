#include "mappers.h"

namespace cf {
namespace PatchMapper {

double map_weight(MappingContext const & context)
{
	return context.conductorContext.power;
}

} // namespace PatchMapper
} // namespace cf
