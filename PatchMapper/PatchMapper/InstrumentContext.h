#pragma once 

#include "cf/time.h"

#include "Data/Instrument.h"

namespace cf {
namespace PatchMapper {

struct InstrumentContext
{
	InstrumentContext(Data::Instrument const & instrument)
		: shortestNoteThreshold(instrument.shortest_note_threshold)
		, longestNoteThreshold(instrument.longest_note_threshold)
	{}

	seconds_t shortestNoteThreshold;
	seconds_t longestNoteThreshold;
};

} // namespace PatchMapper
} // namespace cf