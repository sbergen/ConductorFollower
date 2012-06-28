#pragma once

#include <vector>

#include "Instrument.h"

namespace cf {
namespace Data {

typedef std::vector<Instrument> InstrumentList;

class InstrumentParser
{
public:
	InstrumentParser();

	bool parse(std::string const & filename);
	InstrumentList const & Instruments() const { return data; }

private:
	InstrumentList data;
};

} // namespace Data
} // namespace cf
