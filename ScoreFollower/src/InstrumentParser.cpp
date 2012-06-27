#include "InstrumentParser.h"

#include <fstream>
#include <exception>

#include <boost/spirit/include/support_istream_iterator.hpp>

namespace cf {
namespace ScoreFollower {

InstrumentParser::InstrumentParser(std::string filename)
{
	typedef boost::spirit::basic_istream_iterator<char> iterator_type;
	InstrumentGrammar<iterator_type> grammar;
	
	std::ifstream file(filename);
	file.unsetf(std::ios::skipws);

	iterator_type iter(file);
    iterator_type end;
	bool success = qi::phrase_parse(iter, end, grammar, ascii::space, data);

	if (iter != end) {
		throw std::runtime_error("IntrumentParser did not parse the complete file!");
	}
}

} // namespace ScoreFollower
} // namespace cf
