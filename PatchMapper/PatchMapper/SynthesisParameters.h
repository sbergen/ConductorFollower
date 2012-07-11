#pragma once 

#include "Data/InstrumentPatch.h"

#include <boost/array.hpp>

namespace cf {
namespace PatchMapper {

class SynthesisParameters
{
public:
	typedef boost::array<double, 3> array_type;

	SynthesisParameters(Data::InstrumentPatch const & p)
	{
		data_[0] = p.length;
		data_[1] = p.attack;
		data_[2] = p.weight;
	}

	SynthesisParameters(array_type const & data)
		: data_(data)
	{}

	inline array_type const & data() const { return data_; }

	inline double length() const { return data_[0]; }
	inline double attack() const { return data_[1]; }
	inline double weight() const { return data_[2]; }

private:
	array_type data_;
};

} // namespace PatchMapper
} // namespace cf