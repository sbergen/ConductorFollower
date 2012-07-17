#pragma once 

#include "cf/time.h"

#include "Data/Instrument.h"

namespace cf {
namespace PatchMapper {

namespace si = boost::units::si;

struct InstrumentContext
{
	InstrumentContext(Data::Instrument const & instrument)
		: shortestNoteThreshold(instrument.shortest_note_threshold * si::seconds)
		, longestNoteThreshold(instrument.longest_note_threshold * si::seconds)
	{}

	time_quantity shortestNoteThreshold;
	time_quantity longestNoteThreshold;
};

} // namespace PatchMapper
} // namespace cf