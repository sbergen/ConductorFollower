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

double ComparableDistance(SynthesisParameters const & a, SynthesisParameters const & b)
{
	return 10 * std::pow((a.length() - b.length()), 2.0) + 
				std::pow((a.attack() - b.attack()), 2.0) +
				std::pow((a.weight() - b.weight()), 2.0);

	/*
	SynthesisParameters::array_type diffSquared;
	std::transform(a.data().begin(), a.data().end(), b.data().begin(), diffSquared.begin(),
		[](double lhs, double rhs) -> double { return std::pow((lhs - rhs), 2.0); });
	return std::accumulate(diffSquared.begin(), diffSquared.end(), 0.0, std::plus<double>());
	*/
}

} // namespace PatchMapper
} // namespace cf