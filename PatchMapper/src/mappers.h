#pragma once 

#include "PatchMapper/InstrumentContext.h"
#include "PatchMapper/NoteContext.h"

namespace cf {
namespace PatchMapper {

double map_t_a(InstrumentContext const & instrumentContext, NoteContext const & noteContext);
double map_t_d(InstrumentContext const & instrumentContext, NoteContext const & noteContext);
double map_t_s(InstrumentContext const & instrumentContext, NoteContext const & noteContext);

double map_l_a(InstrumentContext const & instrumentContext, NoteContext const & noteContext);
double map_l_d(InstrumentContext const & instrumentContext, NoteContext const & noteContext);
double map_l_s(InstrumentContext const & instrumentContext, NoteContext const & noteContext);

} // namespace PatchMapper
} // namespace cf