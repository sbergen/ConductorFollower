#pragma once

#include <boost/fusion/adapted.hpp>

#include "Data/Score.h"
#include "Data/ScorePosition.h"
#include "Data/ScoreEvent.h"

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
	(std::string, beatPatternFile)
	(cf::Data::TrackList, tracks)
	(cf::Data::ScoreEventList, events)
)

// Position

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::ScorePosition,
    (int, bar)
	(double, beat)
)

// Events

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::TempoSensitivityChange,
	(cf::Data::ScorePosition, position)
	(double, sensitivity)
)

BOOST_FUSION_ADAPT_STRUCT(
	cf::Data::Fermata,
	(cf::Data::ScorePosition, tempoReference)
	(cf::Data::ScorePosition, position)
	(double, length)
)
