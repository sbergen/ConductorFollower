#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>

namespace cf {
namespace Data {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template <typename Iterator>
struct CommentSkipper : qi::grammar<Iterator>
{
    CommentSkipper() : CommentSkipper::base_type(start)
    {
        using ascii::char_;
		using qi::eol;
		using qi::eps;

		one_line_comment = "//" >> *(char_ - eol) >> eol;
		
		nested_multi_line = eps("/*") >> multi_line_comment >> *(char_ - "*/") >> "*/";
		multi_line_comment = "/*" >> *(char_ - "*/" - "/*") >> ("*/" | nested_multi_line);

		start = qi::space | one_line_comment | multi_line_comment;
    }

	qi::rule<Iterator> start;
    qi::rule<Iterator> one_line_comment;
	qi::rule<Iterator> multi_line_comment;
	qi::rule<Iterator> nested_multi_line;
};

} // namespace Data
} // namespace cf