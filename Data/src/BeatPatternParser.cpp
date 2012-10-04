#include "Data/BeatPatternParser.h"

#include "BeatPatternGrammar.h"
#include "parse.h"

namespace cf {
namespace Data {

BeatPatternParser::BeatPatternParser() {}

void
BeatPatternParser::parse(std::string const & filename)
{
	ParseFromFile<BeatPatternGrammar>(filename, data_);
}

} // namespace Data
} // namespace cf
