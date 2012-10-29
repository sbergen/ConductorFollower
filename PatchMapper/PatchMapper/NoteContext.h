#pragma once 

#include "cf/time.h"

namespace cf {
namespace PatchMapper {

struct NoteContext
{
	NoteContext(time_quantity length, time_quantity timeToNext, double tempo, double velocity)
		: length(length)
		, timeToNext(timeToNext)
		, tempo(tempo)
		, velocity(velocity)
	{}

	time_quantity  length;
	time_quantity  timeToNext;
	double         tempo;
	double         velocity;
};

} // namespace PatchMapper
} // namespace cf