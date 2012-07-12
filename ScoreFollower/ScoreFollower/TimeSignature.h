#pragma once

#include <stdexcept>
#include <string>

#include <boost/format.hpp>

namespace cf {
namespace ScoreFollower {

class TimeSignature
{
public:
	// Default to 4/4
	TimeSignature()
		: count_(4)
		, division_(4)
	{}

	TimeSignature(unsigned count, unsigned division)
		: count_(count)
		, division_(division)
	{
		// we don't support anything too funky
		if (division % 2 != 0) {
			throw std::runtime_error("Invalid division in TimeSignature");
		}
	}

	unsigned count() const { return count_; }
	unsigned division() const { return division_; }

private:
	unsigned count_;
	unsigned division_;
};

} // namespace ScoreFollower
} // namespace cf