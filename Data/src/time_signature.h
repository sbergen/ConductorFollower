#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "cf/TimeSignature.h"

#include "CommentSkipper.h"
#include "ErrorHandler.h"
#include "InstrumentAdapters.h"

namespace cf {
namespace Data {

namespace time_signature {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template<typename Iterator, typename SkipperType>
struct grammar : qi::grammar<Iterator, cf::TimeSignature(), SkipperType>
{
	grammar() : grammar::base_type(start)
	{
		using qi::lit;
		using qi::uint_;
		using qi::_1;
		using qi::_2;

		start = (uint_ >> lit('/') > uint_)
			[ phoenix::construct<cf::TimeSignature>(_1, _2) ];
	}

	qi::rule<Iterator, cf::TimeSignature(), SkipperType> start;
};

} // namespace time_signature
} // namespace Data
} // namespace cf
