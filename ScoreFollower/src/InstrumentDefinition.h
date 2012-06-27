#pragma once

#include <string>
#include <vector>

#include <boost/fusion/include/adapt_struct.hpp>

namespace cf {
namespace ScoreFollower {

struct InstrumentDefinition
{
	InstrumentDefinition() : outChannel(-1) {}

	std::string name;
	std::vector<int> programChanges;
	int outChannel;
};

} // namespace ScoreFollower
} // namespace cf

BOOST_FUSION_ADAPT_STRUCT(
	cf::ScoreFollower::InstrumentDefinition,
    (std::string, name)
    (std::vector<int>, programChanges)
    (int, outChannel)
)
