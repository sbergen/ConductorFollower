#include "PatchMapper/SynthesisMapping.h"

#include <functional>
#include <numeric>

#include <boost/bind/apply.hpp>
#include <boost/bind.hpp>

#include "mappers.h"
#include "MappingContext.h"

namespace cf {
namespace PatchMapper {

SynthesisParameters
SynthParametersFromContexts(InstrumentContext const & instrumentContext, NoteContext const & noteContext)
{
	MappingContext context(instrumentContext, noteContext);

	typedef double (*Transformer)(MappingContext const &);
	boost::array<Transformer, 3> transformation = {
		&map_length,
		&map_attack,
		&map_weight
	};

	SynthesisParameters::array_type result;
	auto transformer = boost::bind(boost::apply<double>(), _1, boost::cref(context));
	std::transform(transformation.begin(), transformation.end(), result.begin(), transformer);
	return result;
}

double ComparableDistance(SynthesisParameters const & a, SynthesisParameters const & b)
{
	SynthesisParameters::array_type diffSquared;
	std::transform(a.data().begin(), a.data().end(), b.data().begin(), diffSquared.begin(),
		[](double lhs, double rhs) -> double { return std::pow((lhs - rhs), 2.0); });
	return std::accumulate(diffSquared.begin(), diffSquared.end(), 0.0, std::plus<double>());
}

} // namespace PatchMapper
} // namespace cf