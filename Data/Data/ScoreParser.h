#pragma once

#include <string>

#include <boost/utility.hpp>

#include "Data/Score.h"

namespace cf {
namespace Data {

class ScoreParser : public boost::noncopyable
{
public:
	ScoreParser();

	bool parse(std::string const & filename);
	Score const & data() const { return data_; }

private:
	Score data_;
};

} // namespace Data
} // namespace cf
