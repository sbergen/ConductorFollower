#include "PatchMapper/SynthesisMapping.h"

#include <functional>
#include <numeric>

#include "mappers.h"
#include "MappingContext.h"

namespace cf {
namespace PatchMapper {

SynthesisParameters
SynthParametersFromContexts(
	InstrumentContext const & instrumentContext,
	NoteContext const & noteContext,
	ConductorContext const & conductorContext)
{
	MappingContext context(instrumentContext, noteContext, conductorContext);

	typedef double (*Transformer)(MappingContext const &);
	boost::array<Transformer, 3> transformation = {
		&map_length,
		&map_attack,
		&map_weight
	};

	SynthesisParameters::array_type result;
	std::transform(transformation.begin(), transformation.end(), result.begin(),
		[&](Transformer const & transformer)
		{
			return (*transformer)(context);
		});
	return result;
}

double ComparableDistance(SynthesisParameters const & params, SynthesisParameters const & target)
{
	double const lengthWeight = 2.5;
	double const weightWeight = 1.0;
	double const attackWeight = 1.0;

	double distance = 0.0;

	// Length
	auto lengthDiff = target.length() - params.length();
	if (lengthDiff > 0) {
		// params shorter than target -> length penalty
		distance += lengthWeight * lengthDiff;
	} else {
		// Less length penalty for too long notes
		distance += 0.4 * lengthWeight * -lengthDiff;
	}

	// Weight
	auto weightDiff = target.weight() - params.weight();
	distance += weightWeight * std::abs(weightDiff);

	// Attack
	auto attackDiff = target.attack() - params.attack();
	distance += attackWeight * std::abs(attackDiff);

	return distance;
}

} // namespace PatchMapper
} // namespace cf