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
		using spirit::_2;
		using spirit::_3;
		using spirit::_4;

		// Supporting rules
        quoted_string = lexeme['"' > *(char_ - '"') > '"'];
		elem_separator = -lit(',');
		name = lit("name") > ':' > quoted_string >> elem_separator;

		// Patch
		keyswitch = lit("keyswitch") > ':' > (keyswitch_str | int_) >> elem_separator;
		length = lit("length") > ':' > double_ >> elem_separator;
		attack = lit("attack") > ':' > double_ >> elem_separator;
		weight = lit("weight") > ':' > double_ >> elem_separator;
		patch_body = name ^ keyswitch ^ length ^ attack ^ weight;
		patch = lit("patch") > '{' > -patch_body > '}';

		// Instrument
		patches = lit("patches") > ':' > "[" > *(patch >> elem_separator) > "]";
		instrument_body = name ^ patches;
		instrument = lit("instrument") > '{' > -instrument_body > '}';

		// Start
		start = "[" > *(instrument[phoenix::bind(insert_instrument, _val, _1)] >> elem_separator) > "]";

		// Error handling
		qi::on_error<qi::fail> (start, error_handler(_4, _3, _2));
    }

	static void insert_instrument(InstrumentMap & map, Instrument const & instrument)
	{
		map[instrument.name] = instrument;
	}

	phoenix::function<error_handler_impl<Iterator> > error_handler;

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
	qi::rule<Iterator, double(), SkipperType> length;
	qi::rule<Iterator, double(), SkipperType> attack;
	qi::rule<Iterator, double(), SkipperType> weight;

	qi::rule<Iterator, InstrumentMap(), SkipperType> start;
};

} // namespace Data
} // namespace cf
