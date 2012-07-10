#pragma once

#include <boost/fusion/adapted.hpp>

#include "Data/Score.h"

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::Track,
    (std::string, name)
	(std::string, instrument)
)

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::Score,
    (std::string, name)
	(std::string, midiFile)
	(std::string, instrumentFile)
	(cf::Data::TrackList, tracks)
)
