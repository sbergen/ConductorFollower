#pragma once

//#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "InstrumentDefinition.h"

namespace cf {
namespace ScoreFollower {

typedef std::vector<InstrumentDefinition> InstrumentDefinitionList;

class InstrumentParser
{
public:
	InstrumentParser(std::string filename);
	InstrumentDefinitionList const & Instruments() const { return data; }

private:
	InstrumentDefinitionList data;
};

/* Boost.Spirit Qi grammar */

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template <typename Iterator>
struct InstrumentGrammar : qi::grammar<Iterator, InstrumentDefinitionList(), ascii::space_type>
{
    InstrumentGrammar() : InstrumentGrammar::base_type(start, "instrument definition list")
    {
        using qi::int_;
        using qi::lit;
        using qi::lexeme;
        using ascii::char_;

		// Grammar definitions

        quoted_string = lexeme['"' > +(char_ - '"') > '"'];
		int_array = '[' > (int_ % ',') > ']';
		
		name = lit("name") >> ':' > quoted_string >> -lit(',');
		program_changes = lit("programChanges") >> ':' > int_array >> -lit(',');
		channel = lit("channel") >> ':' > int_ >> -lit(',');
		content = name ^ program_changes ^ channel;

        start = '[' >> (lit("instrument") >> '{' > content >  '}') % ',' >> ']';

		// Names
		quoted_string.name("string literal");
		int_array.name("integer array");
		name.name("name specification");
		program_changes.name("program change definition");
		channel.name("output channel definition");
		content.name("definition body");
		start.name("list body");

		// Error handling
		using namespace qi::labels;
		using phoenix::construct;
        using phoenix::val;

		qi::on_error<qi::rethrow>
        (
            start
          , std::cout
                << val("* Error, Expecting ")
                << _4                               // what failed?
                << val(" here: '")
                << construct<std::string>(_3, _2)   // iterators to error-pos, end
                << val("'")
                << std::endl
        );
    }

    qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
	qi::rule<Iterator, std::vector<int>(), ascii::space_type> int_array;

	qi::rule<Iterator, std::string(), ascii::space_type> name;
	qi::rule<Iterator, std::vector<int>(), ascii::space_type> program_changes;
	qi::rule<Iterator, int(), ascii::space_type> channel;
	qi::rule<Iterator, InstrumentDefinition(), ascii::space_type> content;

    qi::rule<Iterator, InstrumentDefinitionList(), ascii::space_type> start;
};

} // namespace ScoreFollower
} // namespace cf
