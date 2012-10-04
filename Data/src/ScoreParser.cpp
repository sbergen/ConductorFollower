#include "Data/ScoreParser.h"

#include "ScoreGrammar.h"
#include "parse.h"

namespace cf {
namespace Data {

ScoreParser::ScoreParser() {}

void
ScoreParser::parse(std::string const & filename)
{
	ParseFromFile<ScoreGrammar>(filename, data_);
}

} // namespace Data
} // namespace cf
