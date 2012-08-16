#pragma once 

#include "PatchMapper/InstrumentContext.h"
#include "PatchMapper/NoteContext.h"
#include "PatchMapper/ConductorContext.h"

namespace cf {
namespace PatchMapper {

class MappingContext
{
public:
	MappingContext(
		InstrumentContext const & instrumentContext,
		NoteContext const & noteContext,
		ConductorContext const & conductorContext)
		
		: instrumentContext(instrumentContext)
		, noteContext(noteContext)
		, conductorContext(conductorContext)
		, noteLength(noteContext.length / conductorContext.tempo)
	{}

	InstrumentContext const & instrumentContext;
	NoteContext const & noteContext;
	ConductorContext const & conductorContext;

	time_quantity const noteLength;
};

} // namespace PatchMapper
} // namespace cf