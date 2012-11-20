#include "mappers.h"

namespace cf {
namespace PatchMapper {

double map_weight(MappingContext const & context)
{
	return scale_to_expression_amount(context.conductorContext.weight, context);
}

} // namespace PatchMapper
} // namespace cf
