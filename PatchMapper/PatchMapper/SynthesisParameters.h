#pragma once 

#include <boost/fusion/container/vector.hpp>

namespace cf {
namespace PatchMapper {

namespace fusion = boost::fusion;

typedef fusion::vector6<
	double, // t_a
	double, // t_d
	double, // t_s
	double, // l_a
	double, // l_d
	double  // l_s
	> SynthesisParameters;


} // namespace PatchMapper
} // namespace cf