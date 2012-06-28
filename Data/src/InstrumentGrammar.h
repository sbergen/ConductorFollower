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

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

typedef std::vector<Instrument> InstrumentList;

template <typename Iterator, typename SkipperType = CommentSkipper<Iterator> >
struct InstrumentGrammar : qi::grammar<Iterator, InstrumentList(), SkipperType>
{
    InstrumentGrammar() : InstrumentGrammar::base_type(start, "instrument definition list")
    {
        using qi::int_;
		using qi::double_;
        using qi::lit;
        using qi::lexeme;
        using ascii::char_;

		// Supporting rules
        quoted_string = lexeme['"' > +(char_ - '"') > '"'];
		double_array = '[' > (double_ % ',') > ']';
		elem_separator = -lit(',');
		name = lit("name") > ':' > quoted_string >> elem_separator;

		// Patch
		keyswitch = lit("keyswitch") > ':' > int_ >> elem_separator;
		t_ads = lit("t_ads") > ':' > double_array >> elem_separator;
		l_ads = lit("l_ads") > ':' > double_array >> elem_separator;
		patch_body = name ^ keyswitch ^ t_ads ^ l_ads;
		patch = lit("patch") > '{' > patch_body > '}';

		// Instrument
		patches = lit("patches") > ':' > "[" > (patch % ",") > "]";
		instrument_body = name ^ patches;
		instrument = lit("instrument") > '{' > instrument_body > '}';

		// Start
		start = "[" > (instrument % ',') > "]";

		// Error handling
		qi::on_error<qi::fail> (start, error_handler);
    }

	phoenix::function<error_handler_impl> error_handler;

    qi::rule<Iterator, std::string(), SkipperType> quoted_string;
	qi::rule<Iterator, std::vector<double>(), SkipperType> double_array;
	qi::rule<Iterator, void(), SkipperType> elem_separator;
	
	qi::rule<Iterator, std::string(), SkipperType> name;

	qi::rule<Iterator, Instrument(), SkipperType> instrument;
	qi::rule<Iterator, Instrument(), SkipperType> instrument_body;
	qi::rule<Iterator, std::vector<InstrumentPatch>(), SkipperType> patches;
	
	qi::rule<Iterator, InstrumentPatch(), SkipperType> patch;
	qi::rule<Iterator, InstrumentPatch(), SkipperType> patch_body;
	qi::rule<Iterator, int, SkipperType> keyswitch;
	qi::rule<Iterator, EnvelopeTimes, SkipperType> t_ads;
	qi::rule<Iterator, EnvelopeLevels, SkipperType> l_ads;

	qi::rule<Iterator, InstrumentList(), SkipperType> start;
};

} // namespace Data
} // namespace cf
