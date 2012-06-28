#include "Data/InstrumentParser.h"

#include <fstream>

#include <boost/spirit/include/support_istream_iterator.hpp>

#include "InstrumentGrammar.h"

namespace cf {
namespace Data {

InstrumentParser::InstrumentParser() {}

bool
InstrumentParser::parse(std::string const & filename)
{
	typedef boost::spirit::basic_istream_iterator<char> iterator_type;
	InstrumentGrammar<iterator_type> grammar;
	
	std::ifstream file(filename);
	assert(file.is_open());
	file.unsetf(std::ios::skipws);

	iterator_type iter(file);
    iterator_type end;
	CommentSkipper<iterator_type> skipper;
	bool success = qi::phrase_parse(iter, end, grammar, skipper, data);

	return (success && iter == end);
}

} // namespace Data
} // namespace cf
