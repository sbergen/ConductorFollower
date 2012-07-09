#pragma once 

#include "cf/time.h"

namespace cf {
namespace PatchMapper {

struct NoteContext
{
	NoteContext(microseconds_t length, double velocity)
		: length(length)
		, velocity(velocity)
	{}

	microseconds_t length;
	double         velocity;
};

} // namespace PatchMapper
} // namespace cf