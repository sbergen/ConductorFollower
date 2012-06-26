#pragma once

//#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/karma_strict_relaxed.hpp>

#include "InstrumentDefinition.h"

namespace cf {
namespace ScoreFollower {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template <typename Iterator>
struct InstrumentParser : qi::grammar<Iterator, std::vector<InstrumentDefinition>(), ascii::space_type>
{
    InstrumentParser() : InstrumentParser::base_type(start)
    {
        using qi::int_;
        using qi::lit;
        using qi::lexeme;
        using ascii::char_;

        quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];
		int_array %= '[' >> (int_ % ',') >> ']';
		
		name %= lit("name") >> ':' >> quoted_string >> -lit(',');
		program_changes %= lit("programChanges") >> ':' >> int_array >> -lit(',');
		channel %= lit("channel") >> ':' >> int_ >> -lit(',');
		content %= (name ^ program_changes ^ channel);
		// TODO why does this not work???
		//content %= boost::spirit::strict[name ^ program_changes ^ channel];

        start %= '[' >> (lit("instrument") >> '{' >> content >>  '}') % ',' >> ']';
    }
    qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
	qi::rule<Iterator, std::vector<int>(), ascii::space_type> int_array;

	qi::rule<Iterator, std::string(), ascii::space_type> name;
	qi::rule<Iterator, std::vector<int>(), ascii::space_type> program_changes;
	qi::rule<Iterator, int(), ascii::space_type> channel;
	qi::rule<Iterator, InstrumentDefinition(), ascii::space_type> content;

    qi::rule<Iterator, std::vector<InstrumentDefinition>(), ascii::space_type> start;
};

} // namespace ScoreFollower
} // namespace cf