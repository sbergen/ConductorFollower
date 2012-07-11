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
	{
		auto const actualLength = time::multiply(noteContext.length, noteContext.tempo);
		noteLength = time::duration_cast<seconds_t>(actualLength);
	}

	InstrumentContext const & instrumentContext;
	NoteContext const & noteContext;

	seconds_t noteLength;
};

} // namespace PatchMapper
} // namespace cf