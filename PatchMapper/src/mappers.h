#pragma once 

#include "cf/math.h"

#include "MappingContext.h"

namespace cf {
namespace PatchMapper {

inline double scale_to_expression_amount(double val, MappingContext const & context)
{
	val = 0.5 + context.conductorContext.expressionAmount * (val - 0.5);
	return math::clamp(val, 0.0, 1.0);
}

double map_length(MappingContext const & context);
double map_attack(MappingContext const & context);
double map_weight(MappingContext const & context);


} // namespace PatchMapper
} // namespace cf