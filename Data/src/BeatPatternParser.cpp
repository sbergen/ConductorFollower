#include "Data/BeatPatternParser.h"

#include "BeatPatternGrammar.h"
#include "parse.h"

namespace cf {
namespace Data {

BeatPatternParser::BeatPatternParser() {}

bool
BeatPatternParser::parse(std::string const & filename)
{
	return ParseFromFile<BeatPatternGrammar>(filename, data_);
}

} // namespace Data
} // namespace cf
