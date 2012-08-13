#pragma once 

#include "PatchMapper/ConductorContext.h"
#include "PatchMapper/InstrumentContext.h"
#include "PatchMapper/NoteContext.h"
#include "PatchMapper/SynthesisParameters.h"

namespace cf {
namespace PatchMapper {

SynthesisParameters SynthParametersFromContexts(
	InstrumentContext const & instrumentContext,
	NoteContext const & noteContext,
	ConductorContext const & conductorContext);

double ComparableDistance(SynthesisParameters const & params, SynthesisParameters const & target);

} // namespace PatchMapper
} // namespace cf