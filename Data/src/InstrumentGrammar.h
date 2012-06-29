#pragma once

#include <map>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

#include "CommentSkipper.h"
#include "ErrorHandler.h"
#include "InstrumentAdapters.h"
#include "keyswitch.h"

namespace cf {
namespace Data {

namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;
namespace phoenix = boost::phoenix;

typedef std::vector<Instrument> InstrumentList;
typedef std::map<std::string, Instrument> InstrumentMap;

template <typename Iterator, typename SkipperType = CommentSkipper<Iterator> >
struct InstrumentGrammar : qi::grammar<Iterator, InstrumentMap(), SkipperType>
{
    InstrumentGrammar() : InstrumentGrammar::base_type(start, "instrument definition list")
    {
        using qi::int_;
		using qi::double_;
        using qi::lit;
        using qi::lexeme;
        using ascii::char_;
		using spirit::_val;
		using spirit::_1;

		// Supporting rules
        quoted_string = lexeme['"' > +(char_ - '"') > '"'];
		elem_separator = -lit(',');
		name = lit("name") > ':' > quoted_string >> elem_separator;

		// Patch helpers
		auto double_triple = double_ >> elem_separator >> double_ >> elem_separator >> double_;

		// Patch
		keyswitch = lit("keyswitch") > ':' > (keyswitch_str | int_) >> elem_separator;
		t_ads = lit("t_ads") > ':' > ('[' > double_triple > ']') >> elem_separator;
		l_ads = lit("l_ads") > ':' > ('[' > double_triple > ']') >> elem_separator;
		patch_body = name ^ keyswitch ^ t_ads ^ l_ads;
		patch = lit("patch") > '{' > -patch_body > '}';

		// Instrument
		patches = lit("patches") > ':' > "[" > *(patch >> elem_separator) > "]";
		instrument_body = name ^ patches;
		instrument = lit("instrument") > '{' > -instrument_body > '}';

		// Start
		start = "[" > *(instrument[phoenix::bind(insert_instrument, _val, _1)] >> elem_separator) > "]";

		// Error handling
		qi::on_error<qi::fail> (start, error_handler);
    }

	static void insert_instrument(InstrumentMap & map, Instrument const & instrument)
	{
		map[instrument.name] = instrument;
	}

	phoenix::function<error_handler_impl> error_handler;

    qi::rule<Iterator, std::string(), SkipperType> quoted_string;
	qi::rule<Iterator, void(), SkipperType> elem_separator;
	
	qi::rule<Iterator, std::string(), SkipperType> name;

	qi::rule<Iterator, Instrument(), SkipperType> instrument;
	qi::rule<Iterator, Instrument(), SkipperType> instrument_body;
	qi::rule<Iterator, std::vector<InstrumentPatch>(), SkipperType> patches;
	
	qi::rule<Iterator, InstrumentPatch(), SkipperType> patch;
	qi::rule<Iterator, InstrumentPatch(), SkipperType> patch_body;
	keyswitch::grammar<Iterator, SkipperType> keyswitch_str;
	qi::rule<Iterator, int(), SkipperType> keyswitch;
	qi::rule<Iterator, EnvelopeTimes(), SkipperType> t_ads;
	qi::rule<Iterator, EnvelopeLevels(), SkipperType> l_ads;

	qi::rule<Iterator, InstrumentMap(), SkipperType> start;
};

} // namespace Data
} // namespace cf
