#pragma once

#include <boost/fusion/adapted.hpp>

#include "Data/BeatPattern.h"

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::Beat,
	(double, time)
	(double, direction)
)

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::BeatPattern,
	(cf::TimeSignature, meter)
	(std::vector<cf::Data::Beat>, beats)
)
