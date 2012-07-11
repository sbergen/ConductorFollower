#pragma once

#include <boost/fusion/adapted.hpp>

#include "Data/Instrument.h"

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::InstrumentPatch,
    (std::string, name)
	(int, keyswitch)
    (double, length)
	(double, attack)
	(double, weight)
)

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::Instrument,
	(std::string, name)
	(std::vector<cf::Data::InstrumentPatch>, patches)
)