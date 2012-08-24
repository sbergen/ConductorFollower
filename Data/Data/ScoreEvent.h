#pragma once

#include <boost/variant.hpp>

#include "Data/ScorePosition.h"

namespace cf {
namespace Data {

struct TempoSensitivityChange
{
	ScorePosition position;
	double sensitivity;
};

typedef boost::variant<TempoSensitivityChange> ScoreEvent;

} // namespace Data
} // namespace cf
