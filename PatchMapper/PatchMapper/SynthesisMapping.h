#pragma once 

#include "PatchMapper/InstrumentContext.h"
#include "PatchMapper/NoteContext.h"
#include "PatchMapper/SynthesisParameters.h"

namespace cf {
namespace PatchMapper {

SynthesisParameters SynthParametersFromContexts(InstrumentContext const & instrumentContext, NoteContext const & noteContext);

double ComparableDistance(SynthesisParameters const & a, SynthesisParameters const & b);

} // namespace PatchMapper
} // namespace cf