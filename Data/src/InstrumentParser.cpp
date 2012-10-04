#include "Data/InstrumentParser.h"

#include "InstrumentGrammar.h"
#include "parse.h"

namespace cf {
namespace Data {

InstrumentParser::InstrumentParser() {}

void
InstrumentParser::parse(std::string const & filename)
{
	ParseFromFile<InstrumentGrammar>(filename, data_);
}

} // namespace Data
} // namespace cf
