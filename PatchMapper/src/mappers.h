#pragma once 

#include "MappingContext.h"

namespace cf {
namespace PatchMapper {

double map_length(MappingContext const & context);
double map_attack(MappingContext const & context);
double map_weight(MappingContext const & context);


} // namespace PatchMapper
} // namespace cf