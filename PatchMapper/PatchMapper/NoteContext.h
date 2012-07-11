#pragma once 

#include "cf/time.h"

namespace cf {
namespace PatchMapper {

struct NoteContext
{
	NoteContext(microseconds_t length, double tempo, double velocity)
		: length(length)
		, tempo(tempo)
		, velocity(velocity)
	{}

	microseconds_t length;
	double         tempo;
	double         velocity;
};

} // namespace PatchMapper
} // namespace cf