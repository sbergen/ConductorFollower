#pragma once 

#include "PatchMapper/InstrumentContext.h"
#include "PatchMapper/NoteContext.h"

namespace cf {
namespace PatchMapper {

class MappingContext
{
public:
	MappingContext(InstrumentContext const & instrumentContext, NoteContext const & noteContext)
		: instrumentContext(instrumentContext)
		, noteContext(noteContext)
		, noteLength(noteContext.length * noteContext.tempo)
	{}

	InstrumentContext const & instrumentContext;
	NoteContext const & noteContext;

	time_quantity const noteLength;
};

} // namespace PatchMapper
} // namespace cf