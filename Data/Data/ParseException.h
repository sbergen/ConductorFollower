#pragma once

#include <stdexcept>

namespace cf {
namespace Data {

class ParseException : public std::runtime_error
{
public:
	ParseException(std::string const & what)
		: runtime_error(what)
	{}
};

} // namespace cf
} // namespace Data
