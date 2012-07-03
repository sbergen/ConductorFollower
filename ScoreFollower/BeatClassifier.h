#pragma once

#include <boost/utility.hpp>

namespace cf {
namespace ScoreFollower {

class BeatClassifier : public boost::noncopyable
{
public:

	struct Result
	{
		Result() : eightsSincePrevious(-1), probability(0.0) {}
		Result(int eights, double p) : eightsSincePrevious(eights), probability(p) {}

		bool valid() { return eightsSincePrevious >= 1; }

		int eightsSincePrevious;

		// This is a probablitiy normalized to the maximum of the "winning" PDF,
		// not a real naive bayesian probablitiy!
		double probability;
	};

	static Result ClassifyBeat(double quartersSincePreviousBeat);

private:
	static double ProbabilityAt(double position, int meanAsEights, double deviation, double & normalizedProb);
};

} // namespace ScoreFollower
} // namespace cf