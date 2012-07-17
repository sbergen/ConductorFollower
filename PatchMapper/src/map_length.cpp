#include "mappers.h"

namespace cf {
namespace PatchMapper {

double map_length(MappingContext const & context)
{
	time_quantity const fullScale = context.instrumentContext.longestNoteThreshold - 
									context.instrumentContext.shortestNoteThreshold;
	double factor = (context.noteLength - context.instrumentContext.shortestNoteThreshold) / fullScale;

	// TODO use boost 1.50.0
	factor = (factor < 0.0) ? 0.0 : ((factor > 1.0) ? 1.0 : factor);
	//LOG("Note length: %1% mapped to t_s %2%", noteContext.length, factor);
	return factor;

}

} // namespace PatchMapper
} // namespace cf
