#include "PatchMapper/SynthesisMapping.h"

#include <functional>
#include <numeric>

#include <boost/bind/apply.hpp>
#include <boost/bind.hpp>

#include "mappers.h"

namespace cf {
namespace PatchMapper {

SynthesisParameters
SynthParametersFromContexts(InstrumentContext const & instrumentContext, NoteContext const & noteContext)
{
	typedef double (*Transformer)(InstrumentContext const &, NoteContext const &);
	boost::array<Transformer, 6> transformation = {
		&map_t_a,
		&map_t_d,
		&map_t_s,
		&map_l_a,
		&map_l_d,
		&map_l_s
	};

	SynthesisParameters::array_type result;
	auto transformer = boost::bind(boost::apply<double>(), _1, boost::cref(instrumentContext), boost::cref(noteContext));
	std::transform(transformation.begin(), transformation.end(), result.begin(), transformer);
	return result;
}

double ComparableDistance(SynthesisParameters const & a, SynthesisParameters const & b)
{
	return std::pow((a.data()[2] - b.data()[2]), 2.0);
	/*
	SynthesisParameters::array_type diffSquared;
	std::transform(a.data().begin(), a.data().end(), b.data().begin(), diffSquared.begin(),
		[](double lhs, double rhs) -> double { return std::pow((lhs - rhs), 2.0); });
	return std::accumulate(diffSquared.begin(), diffSquared.end(), 0.0, std::plus<double>());
	*/
}

} // namespace PatchMapper
} // namespace cf