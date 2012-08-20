#pragma once

#include <map>
#include <vector>

#include <boost/utility.hpp>

#include "cf/TimeSignature.h"

#include "Data/BeatPattern.h"

namespace cf {
namespace Data {

typedef std::vector<BeatPattern> BeatPatternGroup;
typedef std::map<TimeSignature, BeatPatternGroup> PatternMap;

class BeatPatternParser : public boost::noncopyable
{
public:
	BeatPatternParser();

	bool parse(std::string const & filename);
	PatternMap const & Patterns() const { return data_; }

private:
	PatternMap data_;
};

} // namespace Data
} // namespace cf
