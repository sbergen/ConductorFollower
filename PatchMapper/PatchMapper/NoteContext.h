#pragma once 

#include "cf/cf.h"

namespace cf {
namespace PatchMapper {

struct NoteContext
{
	milliseconds_t length;
	double         velocity;
};

} // namespace PatchMapper
} // namespace cf