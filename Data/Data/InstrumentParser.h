#pragma once

#include <boost/utility.hpp>

#include <map>
#include <string>

#include "Data/Instrument.h"

namespace cf {
namespace Data {

typedef std::map<std::string, Instrument> InstrumentMap;

class InstrumentParser : public boost::noncopyable
{
public:
	InstrumentParser();

	void parse(std::string const & filename);
	InstrumentMap const & Instruments() const { return data_; }

private:
	InstrumentMap data_;
};

} // namespace Data
} // namespace cf
