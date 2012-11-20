#include "mappers.h"

#include "cf/math.h"

namespace cf {
namespace PatchMapper {

double map_length(MappingContext const & context)
{
	time_quantity const fullScale = context.instrumentContext.longestNoteThreshold - 
									context.instrumentContext.shortestNoteThreshold;
	double factor = (context.noteLength - context.instrumentContext.shortestNoteThreshold) / fullScale;
	return factor;
}

} // namespace PatchMapper
} // namespace cf
