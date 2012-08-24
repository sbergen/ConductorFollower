#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "CommentSkipper.h"
#include "ErrorHandler.h"
#include "ScoreAdapters.h"

namespace cf {
namespace Data {

namespace score_position {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template<typename Iterator, typename SkipperType>
struct grammar : qi::grammar<Iterator, ScorePosition(), SkipperType>
{
	grammar() : grammar::base_type(start)
	{
		using qi::_val;
		using qi::int_;
		using qi::double_;
		using qi::lit;
		using qi::_1;

		start = int_ >> lit('|') > double_;
	}

	
	qi::rule<Iterator, ScorePosition(), SkipperType> start;
};

} // namespace keyswitch
} // namespace Data
} // namespace cf
