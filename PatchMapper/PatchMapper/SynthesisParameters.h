#pragma once 

#include "Data/InstrumentPatch.h"

#include <boost/array.hpp>

namespace cf {
namespace PatchMapper {

class SynthesisParameters
{
public:
	typedef boost::array<double, 6> array_type;

	SynthesisParameters(Data::InstrumentPatch const & p)
	{
		data_[0] = p.envelopeTimes.attack;
		data_[1] = p.envelopeTimes.decay;
		data_[2] = p.envelopeTimes.sustain;
		data_[3] = p.envelopeLevels.attack;
		data_[4] = p.envelopeLevels.sustain;
		data_[5] = p.envelopeLevels.decay;
	}

	SynthesisParameters(array_type const & data)
		: data_(data)
	{}

	array_type const & data() const
	{
		return data_;
	}

private:
	array_type data_;
};

} // namespace PatchMapper
} // namespace cf