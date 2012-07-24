#include "BeatClassifier.h"

#include <boost/math/distributions/normal.hpp>

#include "cf/globals.h"

namespace cf {
namespace ScoreFollower {

BeatClassifier::Result
BeatClassifier::ClassifyBeat(beats_t beatsSincePreviousBeat)
{
	double const eightDeviation = 1.0 / 16;
	double const quarterDeviation = 1.0 / 6;

	int bestPos = -1;
	double bestProb = -1.0;
	double bestProbNormalized = -1.0;

	// TODO optimize out unnecessary estimates

	// First check if this is an eight
	bestPos = 1;
	bestProb = ProbabilityAt(beatsSincePreviousBeat, bestPos, eightDeviation, bestProbNormalized);

	// Now check quarters
	for (int pos = 2; pos < 128; pos += 2) {
		double probNormalized = -1.0;
		double prob = ProbabilityAt(beatsSincePreviousBeat, pos, quarterDeviation, probNormalized);

		if (prob > bestProb) {
			bestPos = pos;
			bestProb = prob;
			bestProbNormalized = probNormalized;
		} else if (bestProb > 0.001) {
			// The condition is here because if there is a long pause in conducting,
			// the rounding errors will prevail, and we will not get the
			// growing probablility we would expect in a "normal" case.
			return Result(beats_t(bestPos * score::eight_notes), bestProbNormalized);
		}
	}

	// TODO stop the whole thing now!
	return Result();
}

double
BeatClassifier::ProbabilityAt(beats_t position, int meanAsEights, double deviation, double & normalizedProb)
{
	double mean = static_cast<double>(meanAsEights) / 2;
	boost::math::normal dist(mean, deviation);
	double prob = boost::math::pdf(dist, position / (1.0 * score::quarter_note));
	normalizedProb = prob / boost::math::pdf(dist, mean);
	return prob * ((1.0 * score::quarter_notes) / (2.0 * position));
}


} // namespace ScoreFollower
} // namespace cf