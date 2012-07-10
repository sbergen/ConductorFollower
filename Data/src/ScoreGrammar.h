#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

#include "CommentSkipper.h"
#include "ErrorHandler.h"
#include "ScoreAdapters.h"

namespace cf {
namespace Data {

namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;
namespace phoenix = boost::phoenix;

template <typename Iterator, typename SkipperType = CommentSkipper<Iterator> >
struct ScoreGrammar : qi::grammar<Iterator, Score(), SkipperType>
{
    ScoreGrammar() : ScoreGrammar::base_type(start, "score definition")
    {
        using qi::int_;
		using qi::double_;
        using qi::lit;
        using qi::lexeme;
        using ascii::char_;
		using spirit::_val;
		using spirit::_1;
		using spirit::_2;
		using spirit::_3;
		using spirit::_4;

		// Supporting rules
        quoted_string = lexeme['"' > *(char_ - '"') > '"'];
		elem_separator = -lit(',');
		name = lit("name") > ':' > quoted_string >> elem_separator;

		// Track
		instrument = lit("instrument") > ':' > quoted_string >> elem_separator;
		track_body = name ^ instrument;
		track = lit("track") > '{' > -track_body > '}';

		// Score
		track_list = lit("tracks") > ':' > '[' > *(track >> elem_separator) > ']' >> elem_separator;
		midi_file = lit("midi_file") > ':' > quoted_string >> elem_separator;
		instrument_file = lit("instrument_file") > ':' > quoted_string >> elem_separator;
		score_body = name ^ midi_file ^ instrument_file ^ track_list;

		// Start
		start = lit("score") > '{' > -score_body > '}';

		// Error handling
		qi::on_error<qi::fail> (start, error_handler(_4, _3, _2));
    }

	phoenix::function<error_handler_impl<Iterator> > error_handler;

    qi::rule<Iterator, std::string(), SkipperType> quoted_string;
	qi::rule<Iterator, void(), SkipperType> elem_separator;
	
	qi::rule<Iterator, std::string(), SkipperType> name;

	qi::rule<Iterator, std::string(), SkipperType> instrument;
	qi::rule<Iterator, Track(), SkipperType> track_body;
	qi::rule<Iterator, Track(), SkipperType> track;
	qi::rule<Iterator, std::vector<Track>(), SkipperType> track_list;
	
	qi::rule<Iterator, std::string(), SkipperType> midi_file;
	qi::rule<Iterator, std::string(), SkipperType> instrument_file;
	qi::rule<Iterator, Score(), SkipperType> score_body;

	qi::rule<Iterator, Score(), SkipperType> start;
};

} // namespace Data
} // namespace cf
