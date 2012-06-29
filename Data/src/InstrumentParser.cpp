#include "Data/InstrumentParser.h"

#include "InstrumentGrammar.h"
#include "parse.h"

namespace cf {
namespace Data {

InstrumentParser::InstrumentParser() {}

bool
InstrumentParser::parse(std::string const & filename)
{
	return ParseFromFile<InstrumentGrammar>(filename, data_);
}

} // namespace Data
} // namespace cf
