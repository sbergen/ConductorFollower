#include "Data/ScoreParser.h"

#include "ScoreGrammar.h"
#include "parse.h"

namespace cf {
namespace Data {

ScoreParser::ScoreParser() {}

bool
ScoreParser::parse(std::string const & filename)
{
	return ParseFromFile<ScoreGrammar>(filename, data_);
}

} // namespace Data
} // namespace cf
