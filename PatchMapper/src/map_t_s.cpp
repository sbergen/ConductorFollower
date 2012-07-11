#include "mappers.h"

#include "cf/time.h"
#include "cf/globals.h"

namespace cf {
namespace PatchMapper {

static const seconds_t fullLengthAfter(0.4);
static const seconds_t zeroLengthBelow(0.05);
static const seconds_t fullScale = fullLengthAfter - zeroLengthBelow;

double map_t_s(InstrumentContext const & instrumentContext, NoteContext const & noteContext)
{
	auto actualLength = time::duration_cast<seconds_t>(time::multiply(noteContext.length, noteContext.tempo));
	double factor = (actualLength - zeroLengthBelow) / fullScale;

	// TODO use boost 1.50.0
	factor = (factor < 0.0) ? 0.0 : ((factor > 1.0) ? 1.0 : factor);
	//LOG("Note length: %1% mapped to t_s %2%", noteContext.length, factor);
	return factor;
}

} // namespace PatchMapper
} // namespace cf