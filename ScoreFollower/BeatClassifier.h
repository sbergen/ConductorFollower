#pragma once

#include <boost/utility.hpp>

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class BeatClassifier : public boost::noncopyable
{
public:

	struct Result
	{
		Result() : beatsSincePrevious(-1 * score::beats), probability(0.0) {}
		Result(beats_t beats, double p) : beatsSincePrevious(beats), probability(p) {}

		bool valid() { return beatsSincePrevious > (0 * score::beats); }

		beats_t beatsSincePrevious;

		// This is a probablitiy normalized to the maximum of the "winning" PDF,
		// not a real naive bayesian probablitiy!
		double probability;
	};

	Result ClassifyBeat(beats_t beatsSincePreviousBeat);

private:
	double ProbabilityAt(beats_t position, int meanAsEights, double deviation, double & normalizedProb);
};

} // namespace ScoreFollower
} // namespace cf
