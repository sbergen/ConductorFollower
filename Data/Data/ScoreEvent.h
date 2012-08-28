#pragma once

#include <boost/variant.hpp>

#include "cf/score_units.h"

#include "Data/ScorePosition.h"

namespace cf {
namespace Data {

struct TempoSensitivityChange
{
	ScorePosition position;
	double sensitivity;
};

struct Fermata
{
	ScorePosition tempoReference;
	ScorePosition position;
	double length;
};

typedef boost::variant<TempoSensitivityChange, Fermata> ScoreEvent;

} // namespace Data
} // namespace cf
