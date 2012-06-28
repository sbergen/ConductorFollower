#pragma once

#include <boost/fusion/adapted.hpp>

#include "Data/Instrument.h"

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::EnvelopeTimes,
    (double, attack)
	(double, decay)
	(double, sustain)
)

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::EnvelopeLevels,
    (double, attack)
	(double, decay)
	(double, sustain)
)

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::InstrumentPatch,
    (std::string, name)
	(int, keyswitch)
    (cf::Data::EnvelopeTimes, envelopeTimes)
	(cf::Data::EnvelopeLevels, envelopeLevels)
)

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::Instrument,
	(std::string, name)
	(std::vector<cf::Data::InstrumentPatch>, patches)
)