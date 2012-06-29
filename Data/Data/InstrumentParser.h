#pragma once

#include <map>
#include <string>

#include "Data/Instrument.h"

namespace cf {
namespace Data {

typedef std::map<std::string, Instrument> InstrumentMap;

class InstrumentParser
{
public:
	InstrumentParser();

	bool parse(std::string const & filename);
	InstrumentMap const & Instruments() const { return data; }

private:
	InstrumentMap data;
};

} // namespace Data
} // namespace cf
