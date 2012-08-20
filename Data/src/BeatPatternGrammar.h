#pragma once

#include <map>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

#include "cf/TimeSignature.h"

#include "CommentSkipper.h"
#include "ErrorHandler.h"
#include "BeatPatternAdapters.h"
#include "time_signature.h"

namespace cf {
namespace Data {

namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;
namespace phoenix = boost::phoenix;

typedef std::vector<BeatPattern> BeatPatternGroup;
typedef std::map<TimeSignature, BeatPatternGroup> PatternMap;

template <typename Iterator, typename SkipperType = CommentSkipper<Iterator> >
struct BeatPatternGrammar : qi::grammar<Iterator, PatternMap(), SkipperType>
{
    BeatPatternGrammar() : BeatPatternGrammar::base_type(start, "beat pattern definition list")
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

		// Beat
		beat_time = lit("time") > ':' > double_ >> elem_separator;
		beat_direction = lit("direction") > ':' > double_ >> elem_separator;
		beat_body = beat_time ^ beat_direction;
		beat = lit("beat") > '{' > beat_body > '}';

		// BeatPattern
		beats = lit("beats") > ':' > "[" > *(beat >> elem_separator) > "]";
		time_sig = lit("meter") > ':' > time_sig_str >> elem_separator;
		beat_pattern_body = time_sig ^ beats;
		beat_pattern = lit("beat_pattern") > '{' > -beat_pattern_body > '}';

		// Start
		start = "[" > *(beat_pattern[phoenix::bind(insert_pattern, _val, _1)] >> elem_separator) > "]";

		// Error handling
		qi::on_error<qi::fail> (start, error_handler(_4, _3, _2));
    }

	static void insert_pattern(PatternMap & map, BeatPattern const & pattern)
	{
		map[pattern.meter].push_back(pattern);
	}

	phoenix::function<error_handler_impl<Iterator> > error_handler;

    qi::rule<Iterator, std::string(), SkipperType> quoted_string;
	qi::rule<Iterator, void(), SkipperType> elem_separator;
	
	qi::rule<Iterator, std::string(), SkipperType> name;

	qi::rule<Iterator, Beat(), SkipperType> beat;
	qi::rule<Iterator, Beat(), SkipperType> beat_body;
	qi::rule<Iterator, double(), SkipperType> beat_time;
	qi::rule<Iterator, double(), SkipperType> beat_direction;

	qi::rule<Iterator, BeatPattern(), SkipperType> beat_pattern;
	qi::rule<Iterator, BeatPattern(), SkipperType> beat_pattern_body;
	time_signature::grammar<Iterator, SkipperType> time_sig_str;
	qi::rule<Iterator, TimeSignature(), SkipperType> time_sig;
	qi::rule<Iterator, std::vector<Beat>(), SkipperType> beats;

	qi::rule<Iterator, PatternMap(), SkipperType> start;
};

} // namespace Data
} // namespace cf
