#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "CommentSkipper.h"
#include "ErrorHandler.h"
#include "InstrumentAdapters.h"

namespace cf {
namespace Data {

namespace keyswitch {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

struct keys_ : qi::symbols<char, int>
{
	keys_()
	{
		add
			("C",  0)
			("C#", 1)
			("D",  2)
			("D#", 3)
			("E",  4)
			("F",  5)
			("F#", 6)
			("G",  7)
			("G#", 8)
			("A",  9)
			("A#", 10)
			("B",  11)
		;
	}
};

template<typename Iterator, typename SkipperType>
struct grammar : qi::grammar<Iterator, int(), SkipperType>
{
	grammar() : grammar::base_type(start)
	{
		using qi::_val;
		using qi::int_;
		using qi::_1;

		start =
				keys  [_val = _1] >>
				int_  [_val += 12 * (_1 + 5)]
			;
	}

	keys_ keys;
	qi::rule<Iterator, int(), SkipperType> start;
};

} // namespace keyswitch
} // namespace Data
} // namespace cf
