#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "CommentSkipper.h"
#include "ErrorHandler.h"
#include "ScoreAdapters.h"
#include "score_position.h"

namespace cf {
namespace Data {

namespace score_event {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template<typename Iterator, typename SkipperType>
struct grammar : qi::grammar<Iterator, ScoreEvent(), SkipperType>
{
	grammar() : grammar::base_type(start)
	{
		using qi::_val;
		using qi::int_;
		using qi::double_;
		using qi::lit;

		elem_separator = -lit(',');
		score_position_ = lit("position") > ':' > score_position_grammar >> elem_separator;

		sensitivity = lit("sensitivity") > ':' > double_ >> elem_separator;
		tempo_sensitivity_body = score_position_ > sensitivity;
		tempo_sensitivity = lit("tempo_sensitivity") > '{' > tempo_sensitivity_body > '}';

		start = tempo_sensitivity;
	}

	qi::rule<Iterator, void(), SkipperType> elem_separator;
	score_position::grammar<Iterator, SkipperType> score_position_grammar;
	qi::rule<Iterator, ScorePosition(), SkipperType> score_position_;

	qi::rule<Iterator, ScoreEvent(), SkipperType> start;

	qi::rule<Iterator, double(), SkipperType> sensitivity;
	qi::rule<Iterator, TempoSensitivityChange(), SkipperType> tempo_sensitivity_body;
	qi::rule<Iterator, TempoSensitivityChange(), SkipperType> tempo_sensitivity;
};

} // namespace keyswitch
} // namespace Data
} // namespace cf
