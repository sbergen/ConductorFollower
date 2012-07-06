#include "PatchMapper/SynthesisMapping.h"

#include <functional>

#include <boost/bind/apply.hpp>
#include <boost/bind.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/container/generation.hpp>
#include <boost/fusion/algorithm/transformation.hpp>
#include <boost/fusion/algorithm/iteration.hpp>

#include "mappers.h"

namespace cf {
namespace PatchMapper {

namespace fusion = boost::fusion;

SynthesisParameters
SynthParametersFromContexts(InstrumentContext const & instrumentContext, NoteContext const & noteContext)
{
	auto transformation = fusion::make_vector(
		&map_t_a,
		&map_t_d,
		&map_t_s,
		&map_l_a,
		&map_l_d,
		&map_l_s
		);

	auto transformer = boost::bind(boost::apply<double>(), _1, boost::cref(instrumentContext), boost::cref(noteContext));
	return fusion::transform(transformation, transformer);
}

/*
double comparable_distance(SynthesisParameters const & a, SynthesisParameters const & b)
{
	auto diff_squared = fusion::transform(a, b, [](double lhs, double rhs) -> double { return std::pow(2, (lhs - rhs)); });
	return fusion::fold(diff_squared, double(), std::plus<double>());
}
*/

} // namespace PatchMapper
} // namespace cf